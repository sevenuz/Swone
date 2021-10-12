#ifndef SWONE_SERVER_SERVER_H
#define SWONE_SERVER_SERVER_H

#include <map>
#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

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
	void runTcpConnection(sf::TcpSocket& client);
	void handleTcpCreateLobby(sf::TcpSocket& socket, Net::Packet& reqPacket);
	void handleTcpLobbyRefresh(sf::TcpSocket& socket, Net::Packet& reqPacket);
	void handleTcpJoinLobbyReq(sf::TcpSocket& socket, Net::Packet& reqPacket);

	void sendTcpJoinLobbyAck(sf::TcpSocket& socket, Net::JoinLobbyAck jla);

	void runUdpSocket();

	SrvSettings settings;

	sf::TcpListener listener;

	std::map<std::string, Lobby*> lobbies;
};

#endif // SWONE_SERVER_SERVER_H

