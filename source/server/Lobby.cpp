#include "server/Lobby.h"

std::string Lobby::generateCode(std::string name)
{
	return md5sum6(name + "#" + std::to_string(Helper::now()));
}

Lobby::Lobby(SrvSettings& settings, Net::CreateLobbyReq ld) :
	m_code(generateCode(ld.name)),
	m_lobbyData(ld)
{
	m_tickDt = sf::seconds(1.0f / (float)settings.getTickRate());
	m_gc.loadScenery(settings.getResourceDirectory(), ld.fileCheck);
	m_gc.spawnBeginningGameObjects();
}

Lobby::~Lobby() {
	stop();
}

void Lobby::startMainLoop()
{
	while(m_run) {

		m_mtx.lock();
		m_lastGameTick = Helper::now();
		m_gc.update(restartClock()); // sets also tickT and chatT

		if(m_tickT >= m_tickDt) {
			m_tickT -= m_tickDt;
			sendState();
		}
		if(m_chatT >= m_chatDt) {
			m_chatT -= m_chatDt;
			sendChat();
		}
		m_mtx.unlock();

		std::this_thread::sleep_for(std::chrono::microseconds(MAIN_SLEEP));
	}
}

sf::Time Lobby::restartClock()
{
	sf::Time ellapsed = clock.restart();
	m_tickT += ellapsed;
	m_chatT += ellapsed;
	return ellapsed;
}

void Lobby::handlePackets()
{
	while(m_run) {
		if(!m_packetsReceive.empty())
		{
			auto pp = m_packetsReceive.front();
			Player& p = *pp.player;
			switch(pp.packet.getType()) {
				case Net::U_TIMESYNC:
					Net::receiveTimeSync(pp.packet, p.getTimeSyncPeer());
					send(Net::sendTimeSync(m_code, p.getTimeSyncPeer()), p);
					break;
				case Net::U_DISCONNECT:
					receiveDisconnect(pp.packet, p);
					break;
				case Net::U_CHAT_MESSAGE_REQ:
					receiveChatMessageReq(pp.packet, p);
					break;
				case Net::U_PLAYER_CONFIG_REQ:
					receivePlayerConfigReq(pp.packet, p);
					break;
				case Net::U_PLAYER_INPUT:
					receivePlayerInput(pp.packet, p);
					break;
			}
			m_packetsReceive.pop();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(PACKETS_SLEEP));
	}
}

void Lobby::send(Net::GamePacket packet, Player& player)
{
	m_packetsSend.push(PacketPair{packet, &player});
}

void Lobby::pushPacketPair(Net::GamePacket packet, Player& player)
{
	m_packetsReceive.push(PacketPair{packet, &player});
}

bool Lobby::hasPacketPair()
{
	return !m_packetsSend.empty();
}

Lobby::PacketPair Lobby::popPacketPair()
{
	auto pp = m_packetsSend.front();
	m_packetsSend.pop();
	return pp;
}

Player& Lobby::registerClient(Player::Connection connection) // TODO prevent random connecting
{
	// TODO improve player registration
	for(Player* c : m_players) {
		if(c->getConnection().address == connection.address && c->getConnection().port == connection.port)
			return *c;
	}
	Player* player = new Player(sf::microseconds(Helper::now()), connection);

	m_mtx.lock();
	m_players.push_back(player);
	m_mtx.unlock();

	Log::ger().log("player registered.");
	return *player;
}

void Lobby::receiveDisconnect(Net::GamePacket packet, Player& c) // TODO
{
	m_mtx.lock();
	for(GameObject* go : c.getPlayers()) {
		m_gc.removeFromGame(go);
	}
	m_players.remove(&c);
	delete &c;
	m_mtx.unlock();
}

void Lobby::receiveChatMessageReq(Net::GamePacket packet, Player& c)
{
	Net::ChatMessageReq cmr;
	packet >> cmr;
	Log::ger().log("Lobby: got msg: " + cmr.message );
	m_gameChat[packet.getTimestamp()] = cmr;
	Net::GamePacket resPacket(Net::U_CHAT_MESSAGE_ACK, m_code, c.getTimeSyncPeer());
	resPacket << Net::ChatMessageAck{
		packet.getTimestamp()
	};
	send(resPacket, c);
}

void Lobby::receivePlayerConfigReq(Net::GamePacket packet, Player& c)
{
	Net::PlayerConfigReq pcr;
	packet >> pcr;
	m_playerConfigs[packet.getTimestamp()] = pcr;
	Net::GamePacket resPacket(Net::U_PLAYER_CONFIG_ACK, m_code, c.getTimeSyncPeer());

	m_mtx.lock();
	resPacket << Net::PlayerConfigAck{
		packet.getTimestamp(),
		c.createOrUpdatePlayer(m_gc, pcr),
		pcr.selection
	};
	m_mtx.unlock();

	send(resPacket, c);
}

void Lobby::receivePlayerInput(Net::GamePacket packet, Player& c)
{
	Net::PlayerInput pi;
	packet >> pi;
	// TODO handles input from same player for different local player wrongly

	Net::Timestamp owd = c.getTimeSyncPeer().getSmoothedOwd();
	Net::Timestamp ts = packet.getTimestamp() - owd;
	Net::Timestamp latency = c.getTimeSyncPeer().getLatency();
	Net::Timestamp targetT = ts - INTERPOLATION_TIME - latency;
	if(latency >= LATENCY_THRESHOLD) {
		Log::ger().log(pi.identifier + " exceeds Latency-Threshold", Log::Label::Warning);
	}
	m_playerInputs[ts] = pi;

	m_mtx.lock();

	// newest gamestate on client before input was sent
	// client timestamp - one way delay = server time stamp
	auto itState = c.getGameStates().upper_bound(ts);
	if(itState != c.getGameStates().begin()) // TODO else?
		itState = std::prev(itState);
	// gamestate from which client is interpolating
	auto bitState = c.getGameStates().upper_bound(targetT);
	if(bitState != c.getGameStates().begin()) // TODO else?
		bitState = std::prev(bitState);

	// synchronize jitter of gamestates on client side
	// but we only have server side clock inaccuracy...
	// if(targetT - bitState->first > ts - itState->first) { } else { }

	// set clients interpolation state to world
	m_gc.applyGameState(*bitState->second);

	// set clients newest gamestate to his local players
	for(Net::GameObjectState& gos : itState->second->players) {
		for(GameObject* p : c.getPlayers()) {
			if(gos.identifier == p->getIdentifier()) {
				m_gc.applyGameObjectState(p, gos);
				break;
			}
		}
	}

	// update difference between clients latest state and the inputs since then
	auto t = itState->first;

	// apply all inputs since clients input time, also noting out of order inputs
	// first it should always be received input
	for(auto it = m_playerInputs.lower_bound(itState->first); it != m_playerInputs.end(); ++it) {
		// newer timestamps are greater then older, so substract older from newer
		m_gc.update(sf::microseconds(it->first - t));
		m_gc.getGameObejctPointer(it->second.identifier)->event(it->second.inputs);
		t = it->first;
	}
	// update to now again
	if (t > m_lastGameTick) {
		m_gc.update(sf::microseconds(Helper::now() - t));
		// restart clock which updates also sendState timer
		// to synchronize with update loop
		restartClock();
	} else {
		m_gc.update(sf::microseconds(m_lastGameTick - t));
	}
	// save clients local players states
	std::vector<std::pair<GameObject*, Net::GameObjectState>> localPlayerStates;
	for(GameObject* p : c.getPlayers()) {
		localPlayerStates.push_back(std::make_pair(p, m_gc.getGameObjectState(p)));
	}
	// bring objects from clients target time into present
	// now all objects are at the same time again
	m_gc.update(sf::microseconds(INTERPOLATION_TIME));
	// apply clients local player states
	for(auto& p : localPlayerStates) {
		m_gc.applyGameObjectState(p.first, p.second);
	}

	m_mtx.unlock();
}

void Lobby::start()
{
	m_run = true;
	Log::ger().log("Lobby#" + m_code + " is listening.");
	std::thread(&Lobby::handlePackets, this).detach();
	startMainLoop();
}

void Lobby::stop()
{
	m_run = false;
}

void Lobby::sendState()
{
	Net::GameState gs = m_gc.getGameState();
	m_gameStates.push_back(gs);
	if((double)sizeof(gs) / sf::UdpSocket::MaxDatagramSize >= UDP_DATA_THRESHOLD)
		Log::ger().log("GameStatePacket exceeds Data-Threshold", Log::Label::Warning);
	for(Player* p : m_players) {
		if(!p->getTimeSyncPeer().isSynchronized())
			continue;
		Net::GamePacket packet(Net::U_GAME_STATE, m_code, p->getTimeSyncPeer());
		packet << gs;
		/*
		if	(packet.getTimestamp() < packet.getLocalTimestamp())
			Log::ger().log("local remote timestamp delta: " + std::to_string(packet.getLocalTimestamp() - packet.getTimestamp()));
		else
			Log::ger().log("local remote timestamp delta: -" + std::to_string(packet.getTimestamp() - packet.getLocalTimestamp()));
		*/
		// TODO should we use local timestamp?
		p->addGameState(packet.getLocalTimestamp(), &m_gameStates.back());
		send(packet, *p);
	}
}

void Lobby::sendChat()
{
	Net::GameChat gc;
	// send last N messages
	if(m_gameChat.size() > 5)
		gc.messages.insert(std::prev(std::prev(std::prev(std::prev(std::prev(m_gameChat.end()))))), m_gameChat.end()); // TODO
	else
		gc.messages.insert(m_gameChat.begin(), m_gameChat.end());
	for(Player* p : m_players) {
		if(!p->getTimeSyncPeer().isSynchronized())
			continue;
		Net::GamePacket packet(Net::U_GAME_CHAT, m_code, p->getTimeSyncPeer());
		packet << gc;
		send(packet, *p);
	}
}

bool Lobby::verifyJoinLobbyReq(Net::JoinLobbyReq jlr)
{
	if(m_players.size() >= MAX_PLAYER_COUNT)
		throw Net::Status{Net::C_INVALID, "verifyJoinLobbyReq: Lobby is full."};
	if(jlr.code != m_code)
		throw Net::Status{Net::C_INVALID, "verifyJoinLobbyReq: Lobby-code wrong."};
	if(jlr.password != m_lobbyData.password)
		throw Net::Status{Net::C_INVALID, "verifyJoinLobbyReq: Lobby-password wrong."};
	return true;
}

Net::JoinLobbyAck Lobby::getJoinLobbyAck()
{
	return Net::JoinLobbyAck{ m_code, m_lobbyData.fileCheck };
}

Net::LobbyStatus Lobby::getLobbyStatus()
{
	return Net::LobbyStatus{
		m_lobbyData.name,
		m_code,
		m_lobbyData.password.empty(),
		(sf::Uint8)m_players.size()
	};
}

const std::string Lobby::getCode() const
{
	return m_code;
}

std::list<Player*>& Lobby::getPlayers()
{
	return m_players;
}

