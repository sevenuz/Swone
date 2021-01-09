#include "server/Server.h"

Server::Server()
{}

Server::~Server() {
	stop();
}

void Server::handleTcpConnections()
{
	while(m_run) {
		sf::TcpSocket client;
		if (listener.accept(client) == sf::Socket::Done) {

			Net::Packet reqPacket;
			if(client.receive(reqPacket) != sf::Socket::Done) {
				Log::ger().log("sendLobbyRequest: Error while Receiving", Log::Label::Error);
				return;
			}

			Net::Packet resPacket;
			switch((int)reqPacket.getType()) {
				case Net::T_CREATE_LOBBY:
					resPacket = handleTcpCreateLobby(reqPacket);
					break;
			}

			if (client.send(resPacket) != sf::Socket::Done) {
				Log::ger().log("handleCreateLobby: Error while Sending", Log::Label::Error);
				return;
			}
			client.disconnect();
		}
	}
}

Net::Packet Server::handleTcpCreateLobby(Net::Packet& reqPacket)
{
	Net::CreateLobbyReq req;
	reqPacket >> req;

	Net::CreateLobbyRes res;

	res.sceneryFile = !fileMap.count(req.fileCheck.sceneryFile.second);
	res.mapFile = !fileMap.count(req.fileCheck.mapFile.second);
	for(auto& p : req.fileCheck.objectFileMap)
		if(!fileMap.count(p.second))
			res.objectFiles.push_back(p.first);
	for(auto& p : req.fileCheck.textureFileMap)
		if(!fileMap.count(p.second))
			res.textureFiles.push_back(p.first);

	if(res.sceneryFile || res.mapFile || res.objectFiles.size()>0 || res.textureFiles.size()>0) {
		// trigger file upload
		Net::Packet resPacket(Net::T_CREATE_LOBBY);
		resPacket << res;
		return resPacket;
	} else {
		// create Lobby
		Lobby* l = new Lobby(settings, req);
		lobbies.push_back(l);
		std::thread(&Lobby::start, l).detach();
		Net::Packet resPacket(Net::T_JOIN_LOBBY_ACK);
		resPacket << l->getJoinLobbyAck();
		return resPacket;
	}
}

void Server::readDirFileHashesRecursive(std::string dir)
{
	Helper::readDirectory(dir,
		[&](tinydir_file& file){
			if(file.name[0] == '0')
				return;
			try {
				if (!file.is_dir) {
					fileMap[md5file(file.path)] = std::string(file.path);
				}
				else {
					if (strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0)
						readDirFileHashesRecursive(std::string(file.path));
				}
			} catch(const std::invalid_argument& ia) {
				Log::ger().log(ia.what(), Log::Label::Error);
			}
		}
	);
}

int Server::start()
{
	readDirFileHashesRecursive(settings.getResourceDirectory());

	if (listener.listen(settings.getPort()) != sf::Socket::Done) {
		Log::ger().log("handleCreateLobby: Failed to bind Port " + std::to_string(settings.getPort()), Log::Label::Error);
	}
	listener.setBlocking(false);

	m_run = true;
	handleTcpConnections();

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
