#include "server/Lobby.h"

std::string Lobby::generateIdentifier(std::string name)
{
	return md5sum6(name + "#" + std::to_string(Helper::now()));
}

Lobby::Lobby(SrvSettings& settings, Net::CreateLobbyReq ld) :
	m_identifier(generateIdentifier(ld.name)),
	m_lobbyData(ld)
{
	m_tickDt = sf::seconds(1.0f / (float)settings.getTickRate());
	std::string path = GameReader::getFileHashes()[ld.fileCheck.sceneryFile.second];
	m_scenery = new Scenery(settings.getResourceDirectory(), Helper::parseFileName(path), GameReader::getSceneryMap(path));
	m_gc.setScenery(m_scenery);
}

Lobby::~Lobby() {
	stop();
	delete m_scenery;
}

void Lobby::startMainLoop()
{
	while(m_run) {
		sf::Time ellapsed = clock.restart();
		m_tickT += ellapsed;

		m_gc.update(ellapsed);

		if(m_tickT >= m_tickDt) {
			m_tickT -= m_tickDt;

			sendState();
		}
	}
}

void Lobby::handleUdpConnections()
{
	while(m_run) {
		Net::Packet reqPacket;
		Player::Connection conncetion;
		if(socket.receive(reqPacket, conncetion.address, conncetion.port) == sf::Socket::Done)
		{
			if(registerClient(conncetion))
				Log::ger().log("player registered.");
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

std::list<Player*>& Lobby::getPlayers()
{
	return m_players;
}

void Lobby::start()
{
	m_run = true;
	if(socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
		Log::ger().log("Lobby: failed to bind Port", Log::Label::Error);
	}
	socket.setBlocking(false);
	m_port = socket.getLocalPort();
	std::thread udpThread(&Lobby::handleUdpConnections, this);
	Log::ger().log("Lobby#" + m_identifier + " is listening.");
	startMainLoop();
	udpThread.join();
}

void Lobby::stop()
{
	m_run = false;
	socket.unbind();
}

void Lobby::sendState()
{
	//for(Player* p : m_players) {
		// TODO sendLobbyState...
		// socket.send(packet, p->getConnection().address, p->getConnection().port);
	//}
}

bool Lobby::verifyJoinLobbyReq(Net::JoinLobbyReq jlr)
{
	return
		m_players.size() < MAX_PLAYER_COUNT && // TODO handle full Lobby different
		jlr.identifier == m_identifier &&
		jlr.password == m_lobbyData.password;
}

Net::JoinLobbyAck Lobby::getJoinLobbyAck()
{
	return Net::JoinLobbyAck{ m_port, m_lobbyData.fileCheck };
}
