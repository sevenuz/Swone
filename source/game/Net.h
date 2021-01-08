#ifndef SWONE_GAME_NET_H
#define SWONE_GAME_NET_H

#include <SFML/Network.hpp>

namespace Net
{
	static const unsigned char T_VOID = 0;
	static const unsigned char T_CREATE_LOBBY = 1;

	class Packet : public sf::Packet {
		private:
			unsigned char m_type = T_VOID;
		public:
			Packet(const unsigned char type);
			Packet();
			virtual const void* onSend(std::size_t& size);
			virtual void onReceive(const void* data, std::size_t size);

			const unsigned char getType();
	};

	// Type: std::map<T, K>
	template<typename T, typename K>
	sf::Packet& operator <<(sf::Packet& packet, const std::map<T, K>& sm);
	template<typename T, typename K>
	sf::Packet& operator >>(sf::Packet& packet, std::map<T, K>& sm);

	struct CreateLobbyRequest {
		std::string name;
		std::string password;
		std::map<std::string, std::string> fileMap;
	};
	sf::Packet& operator <<(sf::Packet& packet, const CreateLobbyRequest& lr);
	sf::Packet& operator >>(sf::Packet& packet, CreateLobbyRequest& lr);

	struct CreateLobbyResponse {
		std::map<std::string, std::string> fileMap;
	};
	sf::Packet& operator <<(sf::Packet& packet, const CreateLobbyResponse& lr);
	sf::Packet& operator >>(sf::Packet& packet, CreateLobbyResponse& lr);
}

#endif // NET_H
