#ifndef SWONE_GAME_NET_H
#define SWONE_GAME_NET_H

#include <SFML/Network.hpp>

#include "util/reader/Reader.h"
#include "util/Log.h"
#include "util/Helper.h"
#include "game/GameReader.h"

namespace Net
{
	typedef unsigned short Port;

	static const unsigned char T_VOID = 0;
	static const unsigned char T_CREATE_LOBBY = 1;
	static const unsigned char T_JOIN_LOBBY_REQ = 2;
	static const unsigned char T_JOIN_LOBBY_ACK = 3;
	static const unsigned char T_FILE = 4;

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

	struct File {
		sf::Uint64 length;
		char* data;
	};
	sf::Packet& operator <<(sf::Packet& packet, const File& f);
	sf::Packet& operator >>(sf::Packet& packet, File& f);

	static constexpr const char* GFC_SCENERY_FILE = "GfcSceneryFile";
	static constexpr const char* GFC_MAP_FILE = "GfcMapFile";
	static constexpr const char* GFC_TEXTURE_FILE_MAP = "GfcTextureFileMap";
	static constexpr const char* GFC_OBJECT_FILE_MAP = "GfcObjectFileMap";
	struct GameFileCheck {
		StringPair sceneryFile; // first: fileName, second: checksum
		StringPair mapFile; // first: fileName, second: checksum
		StringMap textureFileMap; // first: fileName, second: checksum
		StringMap objectFileMap; // first: fileName, second: checksum
	};
	sf::Packet& operator <<(sf::Packet& packet, const GameFileCheck& lr);
	sf::Packet& operator >>(sf::Packet& packet, GameFileCheck& lr);

	struct GameFileCheckAnswer {
		bool sceneryFile; // set if needed
		bool mapFile; // set if needed
		std::vector<std::string> textureFiles; // names of needed files
		std::vector<std::string> objectFiles; // names of needed files
	};
	sf::Packet& operator <<(sf::Packet& packet, const GameFileCheckAnswer& lr);
	sf::Packet& operator >>(sf::Packet& packet, GameFileCheckAnswer& lr);
	typedef GameFileCheckAnswer CreateLobbyRes;

	void sendMissingFiles(sf::TcpSocket& socket, GameFileCheck gfc, GameFileCheckAnswer gfca);
	void receiveMissingFiles(sf::TcpSocket& socket, GameFileCheck gfc, GameFileCheckAnswer gfca, std::string resDir);

	struct CreateLobbyReq {
		std::string name;
		std::string password;
		GameFileCheck fileCheck;
	};
	sf::Packet& operator <<(sf::Packet& packet, const CreateLobbyReq& lr);
	sf::Packet& operator >>(sf::Packet& packet, CreateLobbyReq& lr);

	struct JoinLobbyReq {
		std::string identifier;
		std::string password;
	};
	sf::Packet& operator <<(sf::Packet& packet, const JoinLobbyReq& lr);
	sf::Packet& operator >>(sf::Packet& packet, JoinLobbyReq& lr);

	struct JoinLobbyAck {
		Port port;
		GameFileCheck fileCheck;
	};
	sf::Packet& operator <<(sf::Packet& packet, const JoinLobbyAck& lr);
	sf::Packet& operator >>(sf::Packet& packet, JoinLobbyAck& lr);
}

#endif // NET_H
