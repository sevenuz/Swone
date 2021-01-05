#include "server/Server.h"

Server::Server() : lobby("Swonsch", socket) {
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

int Server::start()
{
	if (socket.bind(settings.getPort()) != sf::Socket::Done)
	{
		Log::ger().log("Failed to bind Port " + std::to_string(settings.getPort()), Log::Label::Error);
	}
	socket.setBlocking(false);
	m_run = true;
	startMainLoop();
	return 0;
}

void Server::stop()
{
	m_run = false;
}
