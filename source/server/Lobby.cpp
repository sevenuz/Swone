#include "server/Lobby.h"

Lobby::Lobby(std::string name, sf::UdpSocket& socket) : m_name(name), m_socket(socket) {}

Lobby::~Lobby() {}

bool Lobby::registerClient(Player::Connection connection)
{
	// TODO improve player registration
	for(Player* c : m_players) {
		if(c->getConnection().address == connection.address && c->getConnection().port == connection.port)
			return false;
	}
	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto time = std::chrono::duration_cast<std::chrono::seconds>(now).count();
	Player* player = new Player(sf::seconds(time), connection);
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
}
