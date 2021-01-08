#include "server/Lobby.h"

Lobby::Lobby(Net::CreateLobbyRequest lobbyData) : m_lobbyData(lobbyData) {}

Lobby::~Lobby() {}

bool Lobby::registerClient(Player::Connection connection)
{
	// TODO improve player registration
	for(Player* c : m_players) {
		if(c->getConnection().address == connection.address && c->getConnection().port == connection.port)
			return false;
	}
	auto now = std::chrono::system_clock::now().time_since_epoch();
	long time = std::chrono::duration_cast<std::chrono::seconds>(now).count();
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

void Lobby::sendState()
{

}

void Lobby::update(sf::Time ellapsed)
{

}
