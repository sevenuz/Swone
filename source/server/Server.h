#ifndef SWONE_SERVER_SERVER_H
#define SWONE_SERVER_SERVER_H

#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <string>
#include <map>
#include <list>

#include "server/SrvSettings.h"
#include "game/GameController.h"
#include "util/Log.h"

class Server {
public:
	Server();
	virtual ~Server();

	int start();
private:
	struct Client {
		sf::IpAddress address;
		unsigned short port = 0;
	};
	struct Lobby {
		std::string name;
		std::list<Client> clients;
		sf::UdpSocket socket;
		GameController gc;
	};

	bool m_run = true;
	void startMainLoop();

	SrvSettings settings;
	sf::Clock clock;
	sf::Time m_tickT;
	sf::Time m_sceneT;
	sf::Time m_tickDt = sf::seconds(1.0f/20.0f);
	sf::Time m_sceneDt = sf::seconds(1.0f/60.0f);

	sf::UdpSocket socket;
	std::list<Client> clients;
};

#endif // SWONE_SERVER_SERVER_H

