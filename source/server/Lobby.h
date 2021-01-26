#ifndef SWONE_SERVER_LOBBY_H
#define SWONE_SERVER_LOBBY_H

#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <string>
#include <list>
#include <map>
#include <thread>

#include <md5.h>

#include "server/Player.h"
#include "server/SrvSettings.h"
#include "util/Log.h"
#include "util/Helper.h"
#include "game/GameController.h"
#include "game/Scenery.h"
#include "game/Net.h"

#define MAX_PLAYER_COUNT 10

class Lobby {
public:
	static std::string generateCode(std::string name);

	Lobby(SrvSettings& settings, Net::CreateLobbyReq m_lobbyData);
	virtual ~Lobby();

	std::list<Player*>& getPlayers();
	void start();
	void stop();

	const std::string getCode() const;

	bool verifyJoinLobbyReq(Net::JoinLobbyReq jlr);
	Net::JoinLobbyAck getJoinLobbyAck();
	Net::LobbyStatus getLobbyStatus();
private:
	void startMainLoop();
	void handleUdpConnections();
	void receiveChatMessageReq(Net::TimePacket packet, Player::Connection c);
	void receivePlayerConfigReq(Net::TimePacket packet, Player::Connection c);
	void receivePlayerInput(Net::TimePacket packet);

	bool registerClient(Player::Connection connection);
	void sendState();
	void sendChat();

	sf::Clock clock;
	sf::Time m_tickT;
	sf::Time m_tickDt = sf::seconds(1.0f/20.0f);
	sf::Time m_chatT;
	sf::Time m_chatDt = sf::seconds(7.0f); // TODO from Srvsettings?

	sf::UdpSocket m_socket;

	const std::string m_code;
	Net::CreateLobbyReq m_lobbyData;
	Net::Port m_port = sf::Socket::AnyPort;

	std::list<Player*> m_players;
	GameController m_gc;

	bool m_run = false;

	std::map<Net::Timestamp, Net::PlayerConfigReq> m_playerConfigs;
	std::map<Net::Timestamp, Net::PlayerInput> m_playerInputs;
	std::map<Net::Timestamp, Net::GameState> m_gameStates;
	std::map<Net::Timestamp, Net::ChatMessageReq> m_gameChat;
};

#endif

