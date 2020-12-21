#ifndef SWONE_SERVER_LOBBY_H
#define SWONE_SERVER_LOBBY_H

#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <string>
#include <list>
#include <chrono>

#include "server/Player.h"
#include "util/Log.h"
#include "game/GameController.h"

class Lobby {
public:
	Lobby(std::string name, sf::UdpSocket& socket);
	virtual ~Lobby();

	bool registerClient(Player::Connection connection);
	std::list<Player*>& getPlayers();
	void start();
private:
	std::string m_name;
	std::list<Player*> m_players;
	sf::UdpSocket& m_socket;
	GameController m_gc;

	bool m_run = false;
};

#endif

