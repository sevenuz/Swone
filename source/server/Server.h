#ifndef SWONE_SERVER_SERVER_H
#define SWONE_SERVER_SERVER_H

#include <vector>
#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include <md5.h>

#include "server/SrvSettings.h"
#include "server/Player.h"
#include "server/Lobby.h"
#include "game/Net.h"
#include "game/GameReader.h"
#include "util/Helper.h"
#include "util/Log.h"

#define RES_DIR_UPLOADS "uploads/"

class Server {
public:
	Server();
	virtual ~Server();

	int start();
	void stop();
private:
	bool m_run = false;
	void handleTcpConnections();
	Net::Packet handleTcpCreateLobby(Net::Packet& reqPacket);

	SrvSettings settings;

	sf::TcpListener listener;

	std::vector<Lobby*> lobbies;
};

#endif // SWONE_SERVER_SERVER_H

