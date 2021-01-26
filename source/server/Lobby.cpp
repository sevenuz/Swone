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

		m_gc.update(ellapsed);

		if(m_tickT >= m_tickDt) {
			m_tickT -= m_tickDt;
			sendState();
		}
		if(m_chatT >= m_chatDt) {
			m_chatT -= m_chatDt;
			sendChat();
		}
	}
}

void Lobby::handleUdpConnections()
{
	while(m_run) {
		Net::TimePacket reqPacket;
		Player::Connection conncetion;
		if(m_socket.receive(reqPacket, conncetion.address, conncetion.port) == sf::Socket::Done)
		{
			if(registerClient(conncetion))
				Log::ger().log("player registered.");
			switch(reqPacket.getType()) {
				case Net::U_CHAT_MESSAGE_REQ:
					receiveChatMessageReq(reqPacket, conncetion);
					break;
				case Net::U_PLAYER_CONFIG_REQ:
					receivePlayerConfigReq(reqPacket, conncetion);
					break;
				case Net::U_PLAYER_INPUT:
					receivePlayerInput(reqPacket);
					break;
			}
		}
	}
}

bool Lobby::registerClient(Player::Connection connection)
{
	// TODO improve player registration
	for(Player* c : m_players) {
		if(c->getConnection().address == connection.address && c->getConnection().port == connection.port)
			return false;
	}
	Player* player = new Player(sf::seconds(Helper::now()), connection);
	m_players.push_back(player);
	return true;
}

void Lobby::receiveChatMessageReq(Net::TimePacket packet, Player::Connection c)
{
	Net::ChatMessageReq cmr;
	packet >> cmr;
	m_gameChat[packet.getTimestamp()] = cmr;
	Net::TimePacket resPacket(Net::U_CHAT_MESSAGE_ACK);
	m_socket.send(resPacket, c.address, c.port);
}

void Lobby::receivePlayerConfigReq(Net::TimePacket packet, Player::Connection c)
{
	Net::PlayerConfigReq pcr;
	packet >> pcr;
	m_playerConfigs[packet.getTimestamp()] = pcr;
	Net::TimePacket resPacket(Net::U_PLAYER_CONFIG_ACK);
	// TODO send game object player identifier, update go
	m_socket.send(resPacket, c.address, c.port);
}

void Lobby::receivePlayerInput(Net::TimePacket packet)
{
	Net::PlayerInput pi;
	packet >> pi;
	m_playerInputs[packet.getTimestamp()] = pi;
	// TODO handle player input in gamecontroller
}

std::list<Player*>& Lobby::getPlayers()
{
	return m_players;
}

void Lobby::start()
{
	m_run = true;
	if(m_socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
		Log::ger().log("Lobby: failed to bind Port", Log::Label::Error);
	}
	m_socket.setBlocking(false);
	m_port = m_socket.getLocalPort();
	std::thread udpThread(&Lobby::handleUdpConnections, this);
	Log::ger().log("Lobby#" + m_code + " is listening.");
	startMainLoop();
	udpThread.join();
}

void Lobby::stop()
{
	m_run = false;
	m_socket.unbind();
}

void Lobby::sendState()
{
	//for(Player* p : m_players) {
		// TODO sendLobbyState...
		// socket.send(packet, p->getConnection().address, p->getConnection().port);
	//}
}

void Lobby::sendChat()
{
	//for(Player* p : m_players) {
		// TODO sendLobbyState...
		// socket.send(packet, p->getConnection().address, p->getConnection().port);
	//}
}

const std::string Lobby::getCode() const
{
	return m_code;
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
	return Net::JoinLobbyAck{ m_port, m_lobbyData.fileCheck };
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
