#include <client/game/NetController.h>
#include <client/Controller.h>

NetController::NetController(Controller& c) :
	m_c(c),
	m_gc(c.getGameController())
{}

NetController::~NetController()
{}

void NetController::handleUdpConnection()
{
	while(m_run) {
		Net::GamePacket reqPacket;
		sf::IpAddress currentIp;
		Net::Port currentPort;
		if(m_socket.receive(reqPacket, currentIp, currentPort) == sf::Socket::Done)
		{
			if(currentIp != m_serverIpAddress || currentPort != m_serverPort) {
				Log::ger().log("Packet received from unexpected location.", Log::Label::Warning);
				continue;
			}
			reqPacket.setTimeSyncPeer(m_timeSyncPeer);
			switch(reqPacket.getType()){
				case Net::U_TIMESYNC:
					Net::receiveTimeSync(reqPacket, m_timeSyncPeer);
					break;
				case Net::U_CHAT_MESSAGE_ACK:
					receiveChatMessageAck(reqPacket);
					break;
				case Net::U_PLAYER_CONFIG_ACK:
					receivePlayerConfigAck(reqPacket);
					break;
				case Net::U_GAME_STATE:
					receiveGameState(reqPacket);
					break;
				case Net::U_GAME_CHAT:
					receiveGameChat(reqPacket);
					break;
			}
		}
		m_lastGameTick = Helper::now();
		sf::Time ellapsed = restartClock();
		updateGameController(ellapsed);
		handleTimeSync();
		checkAcknowledgements();
		std::this_thread::sleep_for(std::chrono::microseconds(NET_MAIN_SLEEP));
	}
}

sf::Time NetController::restartClock()
{
	sf::Time ellapsed = m_clock.restart();
	m_timeSyncT += ellapsed;
	m_timeT += ellapsed;
	return ellapsed;
}

void NetController::updateGameController(sf::Time ellapsed)
{
	if(m_gameStates.size() < 2)
		return;
	std::lock_guard<std::mutex> lock(m_c.gameMutex);

	Net::Timestamp targetT = m_timeSyncPeer.now() - INTERPOLATION_TIME + ellapsed.asMicroseconds();
	Net::Timestamp finishT = targetT + ellapsed.asMicroseconds();

	auto it = m_gameStates.upper_bound(targetT);
	auto bit = it;

	// first received GameState will be skipped and applied when the second is received
	if(bit != m_gameStates.begin() && bit != m_gameStates.end())
		bit = std::prev(bit);
	else
		return;

	if(finishT > it->first) {
		// we reached next game state and can delete older ones
		m_gameStates.erase(m_gameStates.begin(), bit);

		m_gc.update(sf::microseconds(it->first - targetT));
		m_gc.applyGameState(it->second);
		m_gc.update(sf::microseconds(finishT - it->first));

		if(std::next(it) != m_gameStates.end()) {
			auto nit = std::next(it);
			double p = (double)(finishT - it->first) / (double)(nit->first - it->first);
			m_gc.interpolateGameState(nit->second, p);
		}
	} else {
		double p = (double)(targetT - bit->first) / (double)(it->first - bit->first);
		m_gc.applyGameState(bit->second);
		m_gc.interpolateGameState(it->second, p);
		m_gc.update(ellapsed);
	}
}

void NetController::handleTimeSync()
{
	if(m_timeT >= TIMESYNC_SWITCH_THRESHOLD)
		m_timeSyncDt = TIMESYNC_SWITCH_DT;
	while(m_timeSyncT >= m_timeSyncDt) {
		Log::ger().log("latency: " + std::to_string(m_timeSyncPeer.getLatency()) + "us");
		Net::GamePacket packet = Net::sendTimeSync(m_lobbyCode, m_timeSyncPeer);
		m_socket.send(packet , m_serverIpAddress, m_serverPort);
		m_timeSyncT -= m_timeSyncDt;
	}
}

void NetController::sendPlayerInput(Net::PlayerInput gi)
{
	if(!m_timeSyncPeer.isSynchronized()) {
		Log::ger().log("Not synced -> not sent player input!", Log::Label::Warning);
		return;
	}
	Net::GamePacket packet(Net::U_PLAYER_INPUT, m_lobbyCode, m_timeSyncPeer);
	packet << gi;
	// TODO shoulnt we use local timestampt? -> makes it laggy
	m_playerInputs[packet.getTimestamp()] = gi;
	m_socket.send(packet, m_serverIpAddress, m_serverPort);
}

void NetController::sendChatMessageReq(Net::ChatMessageReq cma)
{
	if(!m_timeSyncPeer.isSynchronized()) {
		Log::ger().log("Not synced -> not sent chat message!", Log::Label::Warning);
		return;
	}
	Net::GamePacket packet(Net::U_CHAT_MESSAGE_REQ, m_lobbyCode, m_timeSyncPeer);
	packet << cma;
	m_chatReqs[packet.getTimestamp()] = cma;
	m_ackChecks.push_back(AckCheck{packet, packet.getTimestamp(), 1});
	m_socket.send(packet, m_serverIpAddress, m_serverPort);
}

void NetController::sendPlayerConfigReq(Net::PlayerConfigReq pca, std::function<void(GameObject*)> cb)
{
	if(!m_timeSyncPeer.isSynchronized()) {
		Log::ger().log("Not synced -> not sent player config!", Log::Label::Warning);
		return;
	}
	Net::GamePacket packet(Net::U_PLAYER_CONFIG_REQ, m_lobbyCode, m_timeSyncPeer);
	packet << pca;
	m_playerConfigReqCbs[packet.getTimestamp()] = cb;
	m_playerConfigReqs[packet.getTimestamp()] = pca;
	m_ackChecks.push_back(AckCheck{packet, packet.getTimestamp(), 1});
	m_socket.send(packet, m_serverIpAddress, m_serverPort);
}

void NetController::deleteAcknowledgement(Net::Timestamp t)
{
	m_ackChecks.remove_if(
		[&](const AckCheck ac) -> bool {
			return ac.packet.getTimestamp() == t;
		}
	);
}

void NetController::checkAcknowledgements()
{
	for(AckCheck& a : m_ackChecks) {
		if((m_timeSyncPeer.now()-a.lastSent) >= TIMEOUT) {
			a.attempts++;
			a.lastSent = m_timeSyncPeer.now();
			if(a.attempts > MAX_ATTEMPTS) {
				Log::ger().log("Packet reached MAX_ATTEMPTS: drop", Log::Label::Error);
				deleteAcknowledgement(a.packet.getTimestamp());
			} else {
				m_socket.send(a.packet, m_serverIpAddress, m_serverPort);
			}
		}
	}
}

void NetController::receiveChatMessageAck(Net::GamePacket packet)
{
	Net::ChatMessageAck a;
	packet >> a;
	deleteAcknowledgement(a.stampCheck);
}

void NetController::receivePlayerConfigAck(Net::GamePacket packet)
{
	Net::PlayerConfigAck pca;
	packet >> pca;
	Log::ger().log("receive PlayerConfigAck " + pca.identifier);
	deleteAcknowledgement(pca.stampCheck);

	m_c.gameMutex.lock();
	GameObject* go;
	try {
		// better performace if we iterate only over localPlayer?
		go = m_gc.getGameObejctPointer(pca.identifier);
	} catch(std::out_of_range& e) {
		// PlayerConfig is applied on the server and synchronized over GameState
		go = m_gc.spawnLocalPlayer(pca.identifier, pca.selection);
	}
	m_playerConfigReqCbs[pca.stampCheck](go);
	m_playerConfigReqCbs.erase(pca.stampCheck);
	m_c.gameMutex.unlock();
}

void NetController::receiveGameState(Net::GamePacket packet)
{
	Net::GameState gs;
	packet >> gs;

	Net::Timestamp owd = m_timeSyncPeer.getSmoothedOwd();
	Net::Timestamp ts = packet.getTimestamp();
	Net::Timestamp latency = m_timeSyncPeer.getLatency();
	if(latency >= LATENCY_THRESHOLD) {
		Log::ger().log("GameState exceeds Latency-Threshold", Log::Label::Warning);
	}

	m_c.gameMutex.lock();

	// add interpolation game state in client time
	auto itt = m_gameStates.insert(std::make_pair(ts + owd, gs)).first;
	if(itt != std::prev(m_gameStates.end())) {
		Log::ger().log("GameState received out of order", Log::Label::Warning);
	}

	// delete already applied player inputs
	auto it = m_playerInputs.upper_bound(ts - owd);
	if(it != m_playerInputs.begin())
		m_playerInputs.erase(m_playerInputs.begin(), std::prev(it));

	// apply newest game state to simulate local players
	for(Net::GameObjectState& gos : gs.players) {
		for(GameObject* p : m_gc.getLocalPlayers()) {
			if(gos.identifier == p->getIdentifier()) {
				m_gc.applyGameObjectState(p, gos);
				break;
			}
		}
	}
	// apply all inputs since newest gamestate
	auto t = ts + owd; // clienttime of the new gamestate
	for(auto it2 = m_playerInputs.begin(); it2 != m_playerInputs.end(); ++it2) {
		// apply newest gamestate to local players
		for(GameObject* go : m_gc.getLocalPlayers()) {
			if(it2->second.identifier == go->getIdentifier()) {
				// newer timestamps are greater then older, so substract older from newer
				m_gc.update(sf::microseconds(it2->first - t));
				go->event(it2->second.inputs);
				t = it2->first;
				break;
			}
		}
	}

	if (t > m_lastGameTick) {
		m_gc.update(sf::microseconds(Helper::now() - t));
		restartClock();
	} else {
		m_gc.update(sf::microseconds(m_lastGameTick - t));
	}

	m_c.gameMutex.unlock();
}

void NetController::receiveGameChat(Net::GamePacket packet)
{
	Net::GameChat gc;
	packet >> gc;
	for(auto& p : gc.messages) {
		m_gameChat[p.first] = p.second;
	}
}

void NetController::start(std::string lobbyCode, sf::IpAddress srvIp, Net::Port srvPort)
{
	if(m_run)
		stop();
	m_lobbyCode = lobbyCode;
	m_serverIpAddress = srvIp;
	m_serverPort = srvPort;
	m_run = true;
	if(m_socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
		Log::ger().log("Client: failed to bind Port", Log::Label::Error);
	}
	m_socket.setBlocking(false);
	m_clientPort = m_socket.getLocalPort();
	Log::ger().log("Client is listening: " + m_serverIpAddress.toString() + ":" + std::to_string(m_serverPort));
	handleUdpConnection();
}

void NetController::disconnect()
{
	m_c.gameMutex.lock();
	Net::GamePacket packet(Net::U_DISCONNECT, m_lobbyCode, m_timeSyncPeer);
	m_socket.send(packet, m_serverIpAddress, m_serverPort);
	stop();
	m_c.gameMutex.unlock();
}

void NetController::stop()
{
	m_run = false;
	m_ackChecks.clear();
	m_chatReqs.clear();
	m_playerConfigReqs.clear();
	m_playerInputs.clear();
	m_gameStates.clear();
	m_gameChat.clear();
	m_socket.unbind();
}
