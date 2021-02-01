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
			switch(reqPacket.getType()){
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
		checkAcknowledgements();
		//std::this_thread::sleep_for(200ms); // TODO
	}
}

void NetController::sendPlayerInput(Net::PlayerInput gi)
{
	Net::GamePacket packet(Net::U_PLAYER_INPUT, m_lobbyCode);
	packet << gi;
	m_playerInputs[packet.getTimestamp()] = gi;
	m_socket.send(packet, m_serverIpAddress, m_serverPort);
}

void NetController::sendChatMessageReq(Net::ChatMessageReq cma)
{
	Net::GamePacket packet(Net::U_CHAT_MESSAGE_REQ, m_lobbyCode);
	packet << cma;
	m_chatReqs[packet.getTimestamp()] = cma;
	m_ackChecks.push_back(AckCheck{packet, packet.getTimestamp(), 1});
	m_socket.send(packet, m_serverIpAddress, m_serverPort);
}

void NetController::sendPlayerConfigReq(Net::PlayerConfigReq pca, std::function<void(GameObject*)> cb)
{
	Net::GamePacket packet(Net::U_PLAYER_CONFIG_REQ, m_lobbyCode);
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
		if((Helper::now()-a.lastSent) >= TIMEOUT) {
			a.attempts++;
			a.lastSent = Helper::now();
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

void NetController::applyGameObjectState(GameObject* go, Net::GameObjectState gos)
{
	auto c = go->getConfig();
	c.name = gos.name;
	c.visible = gos.visible;
	c.color = gos.color;
	c.zindex = gos.zindex;
	c.extensionMap = gos.extensionMap;
	c.body.collidableSolid = gos.collidableSolid;
	c.body.collidableUnsolid = gos.collidableUnsolid;
	c.body.rotatable = gos.rotatable;
	c.body.solid = gos.solid;
	c.body.skip = gos.skip;
	c.body.x = gos.x;
	c.body.y = gos.y;
	c.body.orient = gos.orient;
	c.body.vx = gos.vx;
	c.body.vy = gos.vy;
	c.body.av = gos.av;
	go->applyConfig(c);
}

void NetController::receiveGameState(Net::GamePacket packet)
{
	Net::GameState gs;
	packet >> gs;
	m_gameStates[packet.getTimestamp()] = gs;
	m_c.gameMutex.lock();
	for(Net::GameObjectState gos : gs.objects) {
		GameObject* go;
		try {
			go = m_gc.getGameObejctPointer(gos.identifier);
		} catch(std::out_of_range& e) {
			go = m_gc.spawnGameObject(gos.identifier, gos.key);
		}
		applyGameObjectState(go, gos);
	}
	for(Net::GameObjectState gos : gs.players) {
		GameObject* go;
		try {
			go = m_gc.getGameObejctPointer(gos.identifier);
		} catch(std::out_of_range& e) {
			go = m_gc.spawnPlayer(gos.identifier, gos.key);
		}
		applyGameObjectState(go, gos);
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
	sendChatMessageReq(Net::ChatMessageReq{"Client Register Req Message"});
	m_clientPort = m_socket.getLocalPort();
	Log::ger().log("Client is listening: " + m_serverIpAddress.toString() + ":" + std::to_string(m_serverPort));
	handleUdpConnection();
}

void NetController::disconnect()
{
	Net::GamePacket packet(Net::U_DISCONNECT, m_lobbyCode);
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
