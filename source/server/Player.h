#ifndef SWONE_SERVER_PLAYER_H
#define SWONE_SERVER_PLAYER_H

#include <vector>

#include <SFML/System/Time.hpp>
#include <SFML/Network.hpp>

#include "game/Net.h"
#include "game/GameController.h"

class Player {
public:
	struct Connection {
		sf::IpAddress address;
		Net::Port port = 0;
	};

	Player(sf::Time lpt, Connection c);
	virtual ~Player();

	std::string createOrUpdatePlayer(GameController& gc, Net::PlayerConfigReq pcr);

	void addGameState(Net::Timestamp ts, Net::GameState* gs);
	const std::map<Net::Timestamp, Net::GameState*>& getGameStates();
	Net::TimeSyncPeer& getTimeSyncPeer();
	Connection& getConnection();
	std::vector<GameObject*>& getPlayers();
private:
	Net::TimeSyncPeer m_timeSyncPeer;
	sf::Time m_lastPackTime;
	Connection m_connection;
	std::vector<GameObject*> m_players;
	// saved in Lobby but here associated with timestamp
	std::map<Net::Timestamp, Net::GameState*> m_gameStates;
};

#endif // SWONE_SERVER_SERVER_H

