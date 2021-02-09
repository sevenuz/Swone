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
		handleTimeSync();
		checkAcknowledgements();
		//std::this_thread::sleep_for(200ms); // TODO
	}
}

void NetController::handleTimeSync()
{
	sf::Time ellapsed = m_timeSyncClock.restart();
	m_timeSyncT += ellapsed;
	m_timeT += ellapsed;
	if(m_timeT >= TIMESYNC_SWITCH_THRESHOLD)
		m_timeSyncDt = TIMESYNC_SWITCH_DT;
	while(m_timeSyncT >= m_timeSyncDt) {
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

	auto it = m_gameStates.insert(std::make_pair(packet.getTimestamp(), gs)).first;
	if(it != std::prev(m_gameStates.end())) {
		Log::ger().log("GameState received out of order", Log::Label::Warning);
		it = std::prev(m_gameStates.end());
	}

	Net::Timestamp latency = m_timeSyncPeer.getSmoothedOwd();
	if(latency >= LATENCY_THRESHOLD) {
		Log::ger().log("GameState exceeds Latency-Threshold", Log::Label::Warning);
	}

	m_c.gameMutex.lock();

	// checks if the iterator is usable
	// so first received GameState will be skipped and applied when the second is reveived
	if(it != m_gameStates.begin()) {
		// interpolates from the previous received gamestate
		m_gc.interpolateGameState(std::prev(it)->second);
	}

	// apply newest gamestate to local players
	for(GameObject* go : m_gc.getLocalPlayers()) {
		for(Net::GameObjectState gos : it->second.players) {
			if(go->getIdentifier() == gos.identifier) {
				m_gc.interpolateGameObjectState(go, gos);
				break;
			}
		}
		Net::Timestamp ts = it->first;
		// apply all inputs since newest gamestate
		for(auto it2 = m_playerInputs.lower_bound(it->first); it2 != m_playerInputs.end(); ++it2) {
			if(it2->second.identifier == go->getIdentifier()) {
				// newer timestamps are greater then older, so substract older from newer
				m_gc.update(sf::microseconds(it2->first - ts));
				go->event(it2->second.inputs);
				ts = it2->first;
			}
		}
		m_gc.update(sf::microseconds(m_timeSyncPeer.now() - ts)); // TODO should play to present time?
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
	Net::GamePacket packet(Net::U_DISCONNECT, m_lobbyCode, m_timeSyncPeer);
	m_socket.send(packet, m_serverIpAddress, m_serverPort);
	stop();
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
