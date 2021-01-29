#ifndef SWONE_CLIENT_GAME_NET_CONTROLLER_H
#define SWONE_CLIENT_GAME_NET_CONTROLLER_H

#include <thread>
#include <chrono>
#include <map>
#include <string>
#include <list>

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include "game/GameController.h"
#include "game/Net.h"
#include "util/Helper.h"

#define MAX_ATTEMPTS 5
#define TIMEOUT 10 // in seconds

class NetController {
public:
	NetController(GameController& gc);
	virtual ~NetController();

	void start(std::string lobbyCode, sf::IpAddress srvIp, Net::Port srvPort);
	void stop();
	void disconnect();
	void sendPlayerInput(Net::PlayerInput gi);
	void sendChatMessageReq(Net::ChatMessageReq cma);
	void sendPlayerConfigReq(Net::PlayerConfigReq pca);
private:
	void handleUdpConnection();
	void deleteAcknowledgement(Net::Timestamp t);
	void checkAcknowledgements();
	void receiveChatMessageAck(Net::GamePacket packet);
	void receivePlayerConfigAck(Net::GamePacket packet);
	void receiveGameState(Net::GamePacket packet);
	void receiveGameChat(Net::GamePacket packet);

	void applyGameObjectState(GameObject* go, Net::GameObjectState gos);

	GameController& m_gc;
	sf::UdpSocket m_socket;
	bool m_run = false;
	std::string m_lobbyCode;
	sf::IpAddress m_serverIpAddress;
	Net::Port m_serverPort;
	Net::Port m_clientPort;

	struct AckCheck {
		Net::GamePacket packet;
		Net::Timestamp lastSent;
		unsigned char attempts = 0;
	};
	std::list<AckCheck> m_ackChecks;
	std::map<Net::Timestamp, Net::ChatMessageReq> m_chatReqs;
	std::map<Net::Timestamp, Net::PlayerConfigReq> m_playerConfigReqs;

	std::map<Net::Timestamp, Net::PlayerInput> m_playerInputs;
	std::map<Net::Timestamp, Net::GameState> m_gameStates;
	std::map<Net::Timestamp, Net::ChatMessageReq> m_gameChat;
};
#endif
