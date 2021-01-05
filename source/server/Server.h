#ifndef SWONE_SERVER_SERVER_H
#define SWONE_SERVER_SERVER_H

#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include "server/SrvSettings.h"
#include "server/Player.h"
#include "server/Lobby.h"
#include "util/Log.h"

class Server {
public:
	Server();
	virtual ~Server();

	int start();
	void stop();
private:
	bool m_run = false;
	void startMainLoop();

	SrvSettings settings;
	sf::Clock clock;
	sf::Time m_tickT;
	sf::Time m_tickDt = sf::seconds(1.0f/20.0f);

	sf::UdpSocket socket;
	Lobby lobby;
};

#endif // SWONE_SERVER_SERVER_H

