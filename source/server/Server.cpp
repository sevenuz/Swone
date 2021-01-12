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
		Log::ger().log("sendLobbyRequest: Error while Receiving", Log::Label::Error);
		return;
	}

	switch((int)reqPacket.getType()) {
		case Net::T_CREATE_LOBBY:
			handleTcpCreateLobby(socket, reqPacket);
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
		Net::Packet resPacket(Net::T_CREATE_LOBBY);
		resPacket << res;
		if (socket.send(resPacket) != sf::Socket::Done) {
			Log::ger().log("handleCreateLobby: Error while Sending", Log::Label::Error);
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
		Log::ger().log("handleCreateLobby: Error while Sending", Log::Label::Error);
	}
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
			runTcpConnection(client);
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
