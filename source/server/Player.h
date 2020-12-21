#ifndef SWONE_SERVER_PLAYER_H
#define SWONE_SERVER_PLAYER_H

#include <SFML/System/Time.hpp>
#include <SFML/Network.hpp>

class Player {
public:
	struct Connection {
		sf::IpAddress address;
		unsigned short port = 0;
	};

	Player(sf::Time lpt, Connection c);
	virtual ~Player();

	Connection& getConnection();
private:
	sf::Time m_lastPackTime;
	Connection m_connection;
};

#endif // SWONE_SERVER_SERVER_H

