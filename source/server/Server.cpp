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
	Net::CreateLobbyReq req;
	reqPacket >> req;

	Net::CreateLobbyRes res;

	res.sceneryFile = !GameReader::getFileHashes().count(req.fileCheck.sceneryFile.second);
	res.mapFile = !GameReader::getFileHashes().count(req.fileCheck.mapFile.second);
	for(auto& p : req.fileCheck.objectFileMap)
		if(!GameReader::getFileHashes().count(p.second))
			res.objectFiles.push_back(p.first);
	for(auto& p : req.fileCheck.textureFileMap)
		if(!GameReader::getFileHashes().count(p.second))
			res.textureFiles.push_back(p.first);

	if(res.sceneryFile || res.mapFile || res.objectFiles.size()>0 || res.textureFiles.size()>0) {
		// trigger file upload
		Net::Packet resPacket(Net::T_FILE_REQUEST);
		resPacket << res;
		if (socket.send(resPacket) != sf::Socket::Done) {
			throw Net::Status{Net::C_SEND, "handleTcpCreateLobby: Error while sending T_FILE_REQUEST"};
		}
		Net::receiveMissingFiles(socket, req.fileCheck, res, settings.getResourceDirectory());
	}

	// create Lobby
	Lobby* l = new Lobby(settings, req);
	lobbies.push_back(l);
	std::thread(&Lobby::start, l).detach();
	Net::Packet resPacket(Net::T_JOIN_LOBBY_ACK);
	resPacket << l->getJoinLobbyAck();
	if (socket.send(resPacket) != sf::Socket::Done) {
		throw Net::Status{Net::C_SEND, "handleTcpCreateLobby: Error while sending T_JOIN_LOBBY_ACK"};
	}
}

void Server::handleTcpLobbyRefresh(sf::TcpSocket& socket, Net::Packet& reqPacket)
{

	Net::Packet resPacket(Net::T_LOBBY_REFRESH);
	Net::LobbyRefresh lr;
	for(auto* l : lobbies)
		lr.lobbies.push_back(l->getLobbyStatus());
	resPacket << lr;
	if (socket.send(resPacket) != sf::Socket::Done) {
		throw Net::Status{Net::C_SEND, "handleTcpLobbyRefresh: Error while sending T_LOBBY_REFRESH"};
	}
}

void Server::handleTcpJoinLobbyReq(sf::TcpSocket& socket, Net::Packet& reqPacket)
{

}

int Server::start()
{
	if (listener.listen(settings.getPort()) != sf::Socket::Done) {
		Log::ger().log("handleCreateLobby: Failed to bind Port " + std::to_string(settings.getPort()), Log::Label::Error);
	}
	listener.setBlocking(false);

	m_run = true;
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
	for(Lobby* l : lobbies) {
		l->stop();
	}
}
