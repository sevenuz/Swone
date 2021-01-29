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
		sf::Time ellapsed = clock.restart();
		m_tickT += ellapsed;
		m_chatT += ellapsed;

		m_mtx.lock();
		m_gc.update(ellapsed);

		if(m_tickT >= m_tickDt) {
			m_tickT -= m_tickDt;
			sendState();
		}
		if(m_chatT >= m_chatDt) {
			m_chatT -= m_chatDt;
			sendChat();
		}
		m_mtx.unlock();
	}
}

void Lobby::handlePackets()
{
	while(m_run) {
		if(!m_packetsReceive.empty())
		{
			auto pp = m_packetsReceive.front();
			Player& p = registerClient(pp.connection);
			switch(pp.packet.getType()) {
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
					receivePlayerInput(pp.packet);
					break;
			}
			m_packetsReceive.pop();
		}
	}
}

void Lobby::send(Net::GamePacket packet, Player::Connection connection)
{
	m_packetsSend.push(PacketPair{packet, connection});
}

void Lobby::pushPacketPair(Net::GamePacket packet, Player::Connection connection)
{
	m_packetsReceive.push(PacketPair{packet, connection});
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

Player& Lobby::registerClient(Player::Connection connection)
{
	// TODO improve player registration
	for(Player* c : m_players) {
		if(c->getConnection().address == connection.address && c->getConnection().port == connection.port)
			return *c;
	}
	Player* player = new Player(sf::seconds(Helper::now()), connection);

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
		m_gc.removeFromGame(go->getIdentifier());
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
	Net::GamePacket resPacket(Net::U_CHAT_MESSAGE_ACK, m_code);
	resPacket << Net::ChatMessageAck{
		packet.getTimestamp()
	};
	send(resPacket, c.getConnection());
}

void Lobby::receivePlayerConfigReq(Net::GamePacket packet, Player& c)
{
	Net::PlayerConfigReq pcr;
	packet >> pcr;
	m_playerConfigs[packet.getTimestamp()] = pcr;
	Net::GamePacket resPacket(Net::U_PLAYER_CONFIG_ACK, m_code);

	m_mtx.lock();
	resPacket << Net::PlayerConfigAck{
		packet.getTimestamp(),
		c.createOrUpdatePlayer(m_gc, pcr),
		pcr.selection
	};
	m_mtx.unlock();

	send(resPacket, c.getConnection());
}

void Lobby::receivePlayerInput(Net::GamePacket packet)
{
	m_mtx.lock();
	Net::PlayerInput pi;
	packet >> pi;
	m_playerInputs[packet.getTimestamp()] = pi;
	// TODO handle player input in gamecontroller
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

Net::GameObjectState Lobby::getGameObjectState(GameObject* go)
{
		auto c = go->getConfig();
		return Net::GameObjectState{
			go->getIdentifier(),
			m_gc.getGameObejctKey(go->getIdentifier()),
			c.name,
			c.visible,
			c.color,
			c.zindex,
			c.extensionMap,
			c.body.collidableSolid,
			c.body.collidableUnsolid,
			c.body.rotatable,
			c.body.solid,
			c.body.skip,
			c.body.x,
			c.body.y,
			c.body.orient,
			c.body.vx,
			c.body.vy,
			c.body.av
		};
}

void Lobby::sendState()
{
	Net::GameState gs;
	for(GameObject* go : m_gc.getGameObjects()) {
		gs.objects.push_back(getGameObjectState(go));
	}
	for(GameObject* go : m_gc.getRemotePlayers()) {
		gs.players.push_back(getGameObjectState(go));
	}
	Net::GamePacket packet(Net::U_GAME_STATE, m_code);
	packet << gs;
	if((double)packet.getDataSize() / sf::UdpSocket::MaxDatagramSize >= UDP_DATA_THRESHOLD)
		Log::ger().log("Packet exceeds Data-Threshold", Log::Label::Warning);
	for(Player* p : m_players) {
		send(packet, p->getConnection());
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
	Net::GamePacket packet(Net::U_GAME_CHAT, m_code);
	packet << gc;
	for(Player* p : m_players) {
		send(packet, p->getConnection());
	}
}

bool Lobby::verifyJoinLobbyReq(Net::JoinLobbyReq jlr)
{
	if(m_players.size() >= MAX_PLAYER_COUNT)
		throw Net::Status{Net::C_INVALID, "verifyJoinLobbyReq: Lobby is full."};
	if(jlr.code == m_code)
		throw Net::Status{Net::C_INVALID, "verifyJoinLobbyReq: Lobby-code wrong."};
	if(jlr.password == m_lobbyData.password)
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

