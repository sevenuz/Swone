#ifndef SWONE_GAME_NET_H
#define SWONE_GAME_NET_H

#include <SFML/Network.hpp>

#include "util/reader/Reader.h"

namespace Net
{
	static const unsigned char T_VOID = 0;
	static const unsigned char T_CREATE_LOBBY = 1;
	static const unsigned char T_JOIN_LOBBY_REQ = 2;
	static const unsigned char T_JOIN_LOBBY_ACK = 3;

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

	// Type: std::vector<T>
	template<typename T>
	sf::Packet& operator <<(sf::Packet& packet, const std::vector<T>& sm);
	template<typename T>
	sf::Packet& operator >>(sf::Packet& packet, std::vector<T>& sm);

		// Type: std::pair<T, K>
	template<typename T, typename K>
	sf::Packet& operator <<(sf::Packet& packet, const std::pair<T, K>& sm);
	template<typename T, typename K>
	sf::Packet& operator >>(sf::Packet& packet, std::pair<T, K>& sm);

	// Type: std::map<T, K>
	template<typename T, typename K>
	sf::Packet& operator <<(sf::Packet& packet, const std::map<T, K>& sm);
	template<typename T, typename K>
	sf::Packet& operator >>(sf::Packet& packet, std::map<T, K>& sm);

	struct CreateLobbyRequest {
		std::string name;
		std::string password;
		StringPair sceneryFile;
		StringPair mapFile;
		StringMap textureFileMap;
		StringMap objectFileMap;
	};
	sf::Packet& operator <<(sf::Packet& packet, const CreateLobbyRequest& lr);
	sf::Packet& operator >>(sf::Packet& packet, CreateLobbyRequest& lr);

	struct CreateLobbyResponse {
		bool sceneryFile; // set if needed
		bool mapFile; // set if needed
		std::vector<std::string> textureFiles; // set if needed
		std::vector<std::string> objectFiles; // set if needed
	};
	sf::Packet& operator <<(sf::Packet& packet, const CreateLobbyResponse& lr);
	sf::Packet& operator >>(sf::Packet& packet, CreateLobbyResponse& lr);
}

#endif // NET_H
