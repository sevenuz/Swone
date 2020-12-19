#include "server/Server.h"

Server::Server() {
	m_tickDt = sf::seconds(1.0f / (float)settings.getTickRate());
}

Server::~Server() {}

void Server::startMainLoop()
{
	while(m_run) {
		sf::Time ellapsed = clock.restart();
		m_sceneT += ellapsed;
		m_tickT += ellapsed;

		sf::Packet packet;
		Client client;
		if(socket.receive(packet, client.address, client.port) == sf::Socket::Done)
		{
				// not error or not received (sf::Socket::NotReady)
				Log::ger().log("Packet from " + client.address.toString() + ":" + std::to_string(client.port));
				std::cout << packet << std::endl;
				// TODO improve client registration
				for(Client c : clients) {
					if(c.address == client.address && c.port == client.port)
						break;
				}
				clients.push_back(client);
		}

		if(m_sceneT>=m_sceneDt) {
			m_sceneT -= m_sceneDt;
			// TODO srvscene fps from settings
			// use steady time for determenistic and to increase steadyness
			// m_scene.Step(m_sceneDt.asSeconds(), ph::Vec2(0, m_map->getGravity()));
		}

		if(m_tickT>=m_tickDt) {
			m_tickT -= m_tickDt;

			sf::Packet packet;
			packet << "Moin vom Server :)";
			for(Client c : clients) {
				socket.send(packet, c.address, c.port);
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
	startMainLoop();
}
