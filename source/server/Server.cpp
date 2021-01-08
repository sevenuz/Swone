#include "server/Server.h"

Server::Server() :
	lobby("Swonsch", socket)
{
	m_tickDt = sf::seconds(1.0f / (float)settings.getTickRate());
}

Server::~Server() {}

void Server::startMainLoop()
{
	while(m_run) {
		sf::Time ellapsed = clock.restart();
		m_tickT += ellapsed;

		sf::Packet packet;
		Player::Connection conncetion;
		if(socket.receive(packet, conncetion.address, conncetion.port) == sf::Socket::Done)
		{
			// not error or not received (sf::Socket::NotReady)
			Log::ger().log("Packet from " + conncetion.address.toString() + ":" + std::to_string(conncetion.port));
			std::string msg;
			packet >> msg;
			std::cout << msg << std::endl;
			lobby.registerClient(conncetion);
		}

		//lobby.update();

		if(m_tickT>=m_tickDt) {
			m_tickT -= m_tickDt;

			//lobby.sendState();

			sf::Packet packet;
			packet << "Moin vom Server :)";
			for(Player* c : lobby.getPlayers()) {
				socket.send(packet, c->getConnection().address, c->getConnection().port);
			}
		}
	}
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
	Net::CreateLobbyRequest req;
	reqPacket >> req;

	Net::Packet resPacket(Net::T_CREATE_LOBBY);
	Net::CreateLobbyResponse res;

	for(auto& p : req.fileMap)
		if(!m_fileMap.count(p.second))
			res.fileMap[p.first] = settings.getResourceDirectory() + RES_DIR_UPLOADS;

	if(!res.fileMap.size())
		res.fileMap["Todo"] = "Bien :D";

	resPacket << res;
	return resPacket;
}

void Server::readDirFileHashesRecursive(std::string dir)
{
	Helper::readDirectory(dir,
		[&](tinydir_file& file){
			if(file.name[0] == '0')
				return;
			try {
				if (!file.is_dir) {
					m_fileMap[md5file(file.path)] = std::string(file.path);
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

	if (socket.bind(settings.getPort()) != sf::Socket::Done)
	{
		Log::ger().log("Failed to bind Port " + std::to_string(settings.getPort()), Log::Label::Error);
	}
	socket.setBlocking(false);
	if (listener.listen(settings.getPort()) != sf::Socket::Done) {
		Log::ger().log("handleCreateLobby: Failed to bind Port " + std::to_string(settings.getPort()), Log::Label::Error);
	}
	listener.setBlocking(false);

	std::thread tcpThread(&Server::handleTcpConnections, this);

	m_run = true;
	startMainLoop();

	tcpThread.join();
	return 0;
}

void Server::stop()
{
	m_run = false;
	listener.close();
	socket.unbind();
}
