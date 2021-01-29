#include "server/Server.h"

Server::Server()
{
	GameReader::hashResDir(settings.getResourceDirectory()); // TODO here?
}

Server::~Server() {
	stop();
}

void Server::runTcpConnection(sf::TcpSocket& socket)
{
	Net::Packet reqPacket;
	if(socket.receive(reqPacket) != sf::Socket::Done) {
		throw Net::Status{Net::C_RECEIVE, "runTcpConnection: Error while receiving"};
	}

	switch((int)reqPacket.getType()) {
		case Net::T_CREATE_LOBBY:
			handleTcpCreateLobby(socket, reqPacket);
			break;
		case Net::T_LOBBY_REFRESH:
			handleTcpLobbyRefresh(socket, reqPacket);
			break;
		case Net::T_JOIN_LOBBY_REQ:
			handleTcpJoinLobbyReq(socket, reqPacket);
			break;
	}
	socket.disconnect();
}

void Server::handleTcpCreateLobby(sf::TcpSocket& socket, Net::Packet& reqPacket)
{
	if(lobbies.size() == settings.getLobbyMaximum())
		throw Net::Status{Net::C_INVALID, "handleTcpCreateLobby: Server reached maximum amount of lobbies."};
	Net::CreateLobbyReq req;
	reqPacket >> req;

	Net::handleGameFileCheck(socket, req.fileCheck, settings.getResourceDirectory());

	// create Lobby
	Lobby* l = new Lobby(settings, req);
	lobbies[l->getCode()] = l;
	std::thread(&Lobby::start, l).detach();
	sendTcpJoinLobbyAck(socket, l->getJoinLobbyAck());
}

void Server::handleTcpLobbyRefresh(sf::TcpSocket& socket, Net::Packet& reqPacket)
{
	Net::Packet resPacket(Net::T_LOBBY_REFRESH);
	Net::LobbyRefresh lr;
	for(auto& p : lobbies)
		lr.lobbies.push_back(p.second->getLobbyStatus());
	resPacket << lr;
	if (socket.send(resPacket) != sf::Socket::Done) {
		throw Net::Status{Net::C_SEND, "handleTcpLobbyRefresh: Error while sending T_LOBBY_REFRESH"};
	}
}

void Server::handleTcpJoinLobbyReq(sf::TcpSocket& socket, Net::Packet& reqPacket)
{
	Net::JoinLobbyReq jlr;
	reqPacket >> jlr;
	if(lobbies.count(jlr.code)) {
		Lobby* l = lobbies[jlr.code];
		if(l->verifyJoinLobbyReq(jlr))
			sendTcpJoinLobbyAck(socket, l->getJoinLobbyAck());
	} else {
		throw Net::Status{Net::C_INVALID, "handleTcpJoinLobbyReq: Lobby not exists"};
	}
}

void Server::sendTcpJoinLobbyAck(sf::TcpSocket& socket, Net::JoinLobbyAck jla)
{
	Net::Packet resPacket(Net::T_JOIN_LOBBY_ACK);
	resPacket << jla;
	if (socket.send(resPacket) != sf::Socket::Done) {
		throw Net::Status{Net::C_SEND, "sendTcpJoinLobbyAck: Error while sending T_JOIN_LOBBY_ACK"};
	}
	Net::Packet reqPacket;
	if(reqPacket.getType() == Net::T_FILE_REQUEST) {
		Net::GameFileCheckAnswer res;
		resPacket >> res;
		Net::sendMissingFiles(socket, jla.fileCheck, res);
	}
}

void Server::runUdpSocket()
{
	sf::UdpSocket socket;
	if (socket.bind(settings.getPort()) != sf::Socket::Done) {
		Log::ger().log("runUdpSocket: Failed to bind Port " + std::to_string(settings.getPort()), Log::Label::Error);
	}
	socket.setBlocking(false);
	while(m_run) {
		Net::GamePacket reqPacket;
		Player::Connection conncetion;

		for(auto& p : lobbies) {
			if(p.second->hasPacketPair()) {
				auto pp = p.second->popPacketPair();
				socket.send(pp.packet, pp.connection.address, pp.connection.port);
			}
		}
		if(socket.receive(reqPacket, conncetion.address, conncetion.port) == sf::Socket::Done) {
			lobbies[reqPacket.getCode()]->pushPacketPair(reqPacket, conncetion);
		}
	}
	socket.unbind();
}

int Server::start()
{
	if (listener.listen(settings.getPort()) != sf::Socket::Done) {
		Log::ger().log("handleCreateLobby: Failed to bind Port " + std::to_string(settings.getPort()), Log::Label::Error);
	}
	listener.setBlocking(false);

	m_run = true;
	std::thread(&Server::runUdpSocket, this).detach();
	while(m_run) {
		sf::TcpSocket client;
		if (listener.accept(client) == sf::Socket::Done) {
			// TODO handle TcpConnection in different thread?
			// std::thread(&Server::runTcpConnection, client).detach();
			try {
				runTcpConnection(client);
			} catch(Net::Status s) {
				Log::ger().log(std::to_string(s.code) + ": " + s.message, Log::Label::Error);
				Net::Packet p(Net::T_ERROR);
				client.send(p << s);
			}
		}
	}

	return 0;
}

void Server::stop()
{
	m_run = false;
	listener.close();
	for(auto& p : lobbies) {
		p.second->stop();
	}
}
