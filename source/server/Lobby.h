#ifndef SWONE_SERVER_LOBBY_H
#define SWONE_SERVER_LOBBY_H

#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <string>
#include <list>
#include <map>
#include <queue>
#include <mutex>
#include <thread>

#include <md5.h>

#include "server/Player.h"
#include "server/SrvSettings.h"
#include "util/Log.h"
#include "util/Helper.h"
#include "game/GameController.h"
#include "game/Scenery.h"
#include "game/Net.h"

#define UDP_DATA_THRESHOLD 0.8
#define MAX_PLAYER_COUNT 10

// sleep in microseconds
#define MAIN_SLEEP 1000
#define PACKETS_SLEEP 1000

class Lobby {
public:
	static std::string generateCode(std::string name);

	struct PacketPair {
		Net::GamePacket packet;
		Player* player;
	};

	Lobby(SrvSettings& settings, Net::CreateLobbyReq m_lobbyData);
	virtual ~Lobby();

	Player& registerClient(Player::Connection connection);

	void pushPacketPair(Net::GamePacket packet, Player& player);
	bool hasPacketPair();
	PacketPair popPacketPair();

	std::list<Player*>& getPlayers();
	void start();
	void stop();

	const std::string getCode() const;

	bool verifyJoinLobbyReq(Net::JoinLobbyReq jlr);
	Net::JoinLobbyAck getJoinLobbyAck();
	Net::LobbyStatus getLobbyStatus();
private:
	sf::Time restartClock();
	void startMainLoop();
	void handlePackets();
	void receiveDisconnect(Net::GamePacket packet, Player& c);
	void receiveChatMessageReq(Net::GamePacket packet, Player& c);
	void receivePlayerConfigReq(Net::GamePacket packet, Player& c);
	void receivePlayerInput(Net::GamePacket packet, Player& c);

	void sendState();
	void sendChat();

	std::mutex m_mtx;

	void send(Net::GamePacket packet, Player& player);
	std::queue<PacketPair> m_packetsReceive;
	std::queue<PacketPair> m_packetsSend;

	Net::Timestamp m_lastGameTick;
	sf::Clock clock;
	sf::Time m_tickT;
	sf::Time m_tickDt = sf::seconds(1.0f/20.0f);
	sf::Time m_chatT;
	sf::Time m_chatDt = sf::seconds(7.0f); // TODO from Srvsettings?

	const std::string m_code;
	Net::CreateLobbyReq m_lobbyData;
	Net::Port m_port = sf::Socket::AnyPort;

	std::list<Player*> m_players;
	GameController m_gc;

	bool m_run = false;

	std::map<Net::Timestamp, Net::PlayerConfigReq> m_playerConfigs;
	std::map<Net::Timestamp, Net::PlayerInput> m_playerInputs;
	std::map<Net::Timestamp, Net::ChatMessageReq> m_gameChat;
	// save all sent gameStates and save corresponding timestamp in player
	std::list<Net::GameState> m_gameStates;
};

#endif

