#include <client/game/NetController.h>

NetController::NetController(GameController& gc) :
	m_gc(gc)
{}

NetController::~NetController()
{}

void NetController::handleUdpConnection()
{
	while(m_run) {
		Net::TimePacket reqPacket;
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
					receiveAcknowledgement(reqPacket);
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
	Net::TimePacket packet(Net::U_PLAYER_INPUT);
	packet << gi;
	m_playerInputs[packet.getTimestamp()] = gi;
	m_socket.send(packet, m_serverIpAddress, m_serverPort);
}

void NetController::sendChatMessageReq(Net::ChatMessageReq cma)
{
	Net::TimePacket packet(Net::U_CHAT_MESSAGE_REQ);
	packet << cma;
	m_chatReqs[packet.getTimestamp()] = cma;
	m_ackChecks.emplace_back(packet, packet.getTimestamp(), 1);
	m_socket.send(packet, m_serverIpAddress, m_serverPort);
}

void NetController::sendPlayerConfigReq(Net::PlayerConfigReq pca)
{
	Net::TimePacket packet(Net::U_PLAYER_CONFIG_REQ);
	packet << pca;
	m_playerConfigReqs[packet.getTimestamp()] = pca;
	m_ackChecks.emplace_back(packet, packet.getTimestamp(), 1);
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
				continue;
			}
			m_socket.send(a.packet, m_serverIpAddress, m_serverPort);
		}
	}
}

void NetController::receiveAcknowledgement(Net::TimePacket packet)
{
	Net::Acknowledgement a;
	packet >> a;
	deleteAcknowledgement(a.stampCheck);
}

void NetController::receivePlayerConfigAck(Net::TimePacket packet)
{
	Net::PlayerConfigAck pca;
	packet >> pca;
	deleteAcknowledgement(pca.stampCheck);
	// TODO create player if identifier has no match yet
}

void NetController::receiveGameState(Net::TimePacket packet)
{
	Net::GameState gs;
	packet >> gs;
	m_gameStates[packet.getTimestamp()] = gs;
	// TODO update GameController
}

void NetController::receiveGameChat(Net::TimePacket packet)
{

	Net::GameChat gc;
	packet >> gc;
	m_gameChat[packet.getTimestamp()] = gc;
	// TODO merge chat messages
}

void NetController::start(sf::IpAddress srvIp, Net::Port srvPort)
{
	m_serverIpAddress = srvIp;
	m_serverPort = srvPort;
	m_run = true;
	if(m_socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
		Log::ger().log("Client: failed to bind Port", Log::Label::Error);
	}
	m_socket.setBlocking(false);
	m_clientPort = m_socket.getLocalPort();
	std::thread udpThread(&NetController::handleUdpConnection, this);
	Log::ger().log("Client is listening.");
	udpThread.join();
}

void NetController::stop()
{
	m_run = false;
	m_socket.unbind();
}
