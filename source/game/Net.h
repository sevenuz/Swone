#ifndef SWONE_GAME_NET_H
#define SWONE_GAME_NET_H

#include <SFML/Config.hpp>
#include <SFML/Network.hpp>
#include <vector>

#include "util/reader/Reader.h"
#include "util/Log.h"
#include "util/Helper.h"
#include "game/GameReader.h"

namespace Net
{
	typedef unsigned short Port;

	// Packet Types
	static const unsigned char T_VOID = 0;
	static const unsigned char T_CREATE_LOBBY = 1;
	static const unsigned char T_JOIN_LOBBY_REQ = 2;
	static const unsigned char T_JOIN_LOBBY_ACK = 3;
	static const unsigned char T_FILE = 4;
	static const unsigned char T_FILE_REQUEST = 5;
	static const unsigned char T_ERROR = 6;
	static const unsigned char T_LOBBY_REFRESH = 7;

	// Status Codes
	static const unsigned char C_CONNECTION = 1;
	static const unsigned char C_SEND = 2;
	static const unsigned char C_RECEIVE = 3;
	static const unsigned char C_CORRUPT = 4;
	static const unsigned char C_TYPE = 5;

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

	struct Status {
		sf::Uint16 code;
		std::string message;
	};
	sf::Packet& operator <<(sf::Packet& packet, const Status& f);
	sf::Packet& operator >>(sf::Packet& packet, Status& f);

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
		std::string code;
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

	struct LobbyStatus {
		std::string name;
		std::string code;
		bool hasPassword;
		sf::Uint8 playerCount;
	};
	sf::Packet& operator <<(sf::Packet& packet, const LobbyStatus& lr);
	sf::Packet& operator >>(sf::Packet& packet, LobbyStatus& lr);

	struct LobbyRefresh {
		std::vector<LobbyStatus> lobbies;
	};
	sf::Packet& operator <<(sf::Packet& packet, const LobbyRefresh& lr);
	sf::Packet& operator >>(sf::Packet& packet, LobbyRefresh& lr);
}

#endif // NET_H
