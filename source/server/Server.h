#ifndef SWONE_SERVER_SERVER_H
#define SWONE_SERVER_SERVER_H

#include <thread>
#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include <md5.h>

#include "server/SrvSettings.h"
#include "server/Player.h"
#include "server/Lobby.h"
#include "game/Net.h"
#include "util/reader/Reader.h"
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
	void startMainLoop();
	void handleTcpConnections();
	Net::Packet handleTcpCreateLobby(Net::Packet& reqPacket);
	void readDirFileHashesRecursive(std::string dir);

	SrvSettings settings;
	sf::Clock clock;
	sf::Time m_tickT;
	sf::Time m_tickDt = sf::seconds(1.0f/20.0f);

	sf::TcpListener listener;
	sf::UdpSocket socket;

	Lobby lobby;
	StringMap m_fileMap;
};

#endif // SWONE_SERVER_SERVER_H

