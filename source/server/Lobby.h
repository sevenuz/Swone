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
#include "game/Net.h"

class Lobby {
public:
	Lobby(Net::CreateLobbyRequest m_lobbyData);
	virtual ~Lobby();

	bool registerClient(Player::Connection connection);
	std::list<Player*>& getPlayers();
	void start();
	void sendState();
	void update(sf::Time ellapsed);
private:
	Net::CreateLobbyRequest m_lobbyData;

	std::list<Player*> m_players;
	GameController m_gc;

	bool m_run = false;
};

#endif

