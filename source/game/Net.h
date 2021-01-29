#ifndef SWONE_GAME_NET_H
#define SWONE_GAME_NET_H

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Network.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Utf.hpp>
#include <vector>

#include "imgui.h"
#include "util/reader/Reader.h"
#include "util/Log.h"
#include "util/Helper.h"
#include "game/GameReader.h"
#include "game/object/GameObject.h"

namespace Net
{
	typedef sf::Uint16 Port;
	typedef sf::Uint64 Timestamp;

	// Packet Types TCP
	static const unsigned char T_VOID = 0;
	static const unsigned char T_CREATE_LOBBY = 1;
	static const unsigned char T_JOIN_LOBBY_REQ = 2;
	static const unsigned char T_JOIN_LOBBY_ACK = 3;
	static const unsigned char T_FILE = 4;
	static const unsigned char T_FILE_REQUEST = 5;
	static const unsigned char T_ERROR = 6;
	static const unsigned char T_LOBBY_REFRESH = 7;
	// Packet Types UDP
	static const unsigned char U_PLAYER_CONFIG_REQ = 8;
	static const unsigned char U_PLAYER_CONFIG_ACK = 9;
	static const unsigned char U_PLAYER_INPUT = 10;
	static const unsigned char U_CHAT_MESSAGE_REQ = 11;
	static const unsigned char U_CHAT_MESSAGE_ACK = 12;
	static const unsigned char U_GAME_STATE = 13;
	static const unsigned char U_GAME_CHAT = 14;
	static const unsigned char U_DISCONNECT = 15;

	// Status Codes
	static const unsigned char C_CONNECTION = 1;
	static const unsigned char C_SEND = 2;
	static const unsigned char C_RECEIVE = 3;
	static const unsigned char C_CORRUPT = 4;
	static const unsigned char C_TYPE = 5;
	static const unsigned char C_INVALID = 6;

	class Packet : public sf::Packet {
		private:
			sf::Uint8 m_type = T_VOID;
		public:
			Packet(const sf::Uint8 type);
			Packet();
			virtual const void* onSend(std::size_t& size);
			virtual void onReceive(const void* data, std::size_t size);

			const sf::Uint8 getType();
	};

	class GamePacket : public Net::Packet {
		private:
			Net::Timestamp m_stamp;
			std::string m_code;
		public:
			GamePacket(const sf::Uint8 type, const std::string& code);
			GamePacket();
			virtual const void* onSend(std::size_t& size);
			virtual void onReceive(const void* data, std::size_t size);

			const Net::Timestamp getTimestamp() const;
			const std::string getCode() const;
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

	// Type: sf::Color
	sf::Packet& operator <<(sf::Packet& packet, const sf::Color& sm);
	sf::Packet& operator >>(sf::Packet& packet, sf::Color& sm);

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

	void handleGameFileCheck(sf::TcpSocket& socket, const GameFileCheck& gfc, const std::string& resDir);
	void sendMissingFiles(sf::TcpSocket& socket, GameFileCheck gfc, GameFileCheckAnswer gfca);
	void receiveMissingFiles(sf::TcpSocket& socket, GameFileCheck gfc, GameFileCheckAnswer gfca, const std::string& resDir);

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
		std::string code;
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

	struct ChatMessageReq {
		std::string message;
	};
	sf::Packet& operator <<(sf::Packet& packet, const ChatMessageReq& f);
	sf::Packet& operator >>(sf::Packet& packet, ChatMessageReq& f);

	struct ChatMessageAck {
		sf::Uint64 stampCheck;
	};
	sf::Packet& operator <<(sf::Packet& packet, const ChatMessageAck& f);
	sf::Packet& operator >>(sf::Packet& packet, ChatMessageAck& f);

	struct PlayerConfigReq {
		// is used to update config, if not set, the server creates a player and returns new identifier in Ack
		std::string identifier;
		std::string selection;
		std::string name;
		sf::Color color;
	};
	sf::Packet& operator <<(sf::Packet& packet, const PlayerConfigReq& f);
	sf::Packet& operator >>(sf::Packet& packet, PlayerConfigReq& f);

	struct PlayerConfigAck {
		sf::Uint64 stampCheck;
		std::string identifier;
		std::string selection;
	};
	sf::Packet& operator <<(sf::Packet& packet, const PlayerConfigAck& f);
	sf::Packet& operator >>(sf::Packet& packet, PlayerConfigAck& f);

	struct GameObjectState {
		std::string identifier; // key to the setupMap in scenery
		std::string key; // key to the setupMap in scenery
		std::string name;
		bool visible;
		sf::Color color;
		int zindex;
		StringMapMap extensionMap;
		// body properties
		bool collidableSolid = true;
		bool collidableUnsolid = true;
		bool rotatable = true;
		bool solid = false;
		bool skip = false;
		float x = 0, y = 0;
		float orient = 0;
		float vx = 0, vy = 0, av = 0;
	};
	sf::Packet& operator <<(sf::Packet& packet, const GameObjectState& f);
	sf::Packet& operator >>(sf::Packet& packet, GameObjectState& f);

	struct GameState {
		std::vector<GameObjectState> objects;
		std::vector<GameObjectState> players;
	};
	sf::Packet& operator <<(sf::Packet& packet, const GameState& f);
	sf::Packet& operator >>(sf::Packet& packet, GameState& f);

	struct PlayerInput {
		std::string identifier;
		sf::Uint8 inputs;
	};
	sf::Packet& operator <<(sf::Packet& packet, const PlayerInput& f);
	sf::Packet& operator >>(sf::Packet& packet, PlayerInput& f);

	struct GameChat {
		std::map<Net::Timestamp, ChatMessageReq> messages;
	};
	sf::Packet& operator <<(sf::Packet& packet, const GameChat& f);
	sf::Packet& operator >>(sf::Packet& packet, GameChat& f);
}

#endif // NET_H
