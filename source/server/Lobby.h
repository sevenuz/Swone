#ifndef SWONE_SERVER_LOBBY_H
#define SWONE_SERVER_LOBBY_H

#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <string>
#include <list>
#include <thread>

#include <md5.h>

#include "server/Player.h"
#include "server/SrvSettings.h"
#include "util/Log.h"
#include "util/Helper.h"
#include "game/GameController.h"
#include "game/Net.h"

#define MAX_PLAYER_COUNT 10

class Lobby {
public:
	static std::string generateIdentifier(std::string name);

	Lobby(SrvSettings& settings, Net::CreateLobbyReq m_lobbyData);
	virtual ~Lobby();

	bool registerClient(Player::Connection connection);
	std::list<Player*>& getPlayers();
	void start();
	void stop();
	void sendState();

	bool verifyJoinLobbyReq(Net::JoinLobbyReq jlr);
	Net::JoinLobbyAck getJoinLobbyAck();
private:
	void startMainLoop();
	void handleUdpConnections();

	sf::Clock clock;
	sf::Time m_tickT;
	sf::Time m_tickDt = sf::seconds(1.0f/20.0f);

	sf::UdpSocket socket;

	const std::string m_identifier;
	Net::CreateLobbyReq m_lobbyData;
	Net::Port m_port = sf::Socket::AnyPort;

	std::list<Player*> m_players;
	GameController m_gc;

	bool m_run = false;
};

#endif

