#ifndef SWONE_CLIENT_GAME_NET_CONTROLLER_H
#define SWONE_CLIENT_GAME_NET_CONTROLLER_H

#include <thread>
#include <chrono>
#include <map>
#include <string>
#include <list>
#include <functional>

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include "client/game/ClientGameController.h"
#include "game/Net.h"
#include "util/Helper.h"

#define MAX_ATTEMPTS 5
#define TIMEOUT 10000000 // in microseconds

#define TIMESYNC_START_DT sf::seconds(1.0f/2.0f)
#define TIMESYNC_SWITCH_THRESHOLD sf::seconds(30.0f)
#define TIMESYNC_SWITCH_DT sf::seconds(3.0f)
class Controller;

class NetController {
public:
	NetController(Controller& c);
	virtual ~NetController();

	void start(std::string lobbyCode, sf::IpAddress srvIp, Net::Port srvPort);
	void stop();
	void disconnect();
	void sendPlayerInput(Net::PlayerInput gi);
	void sendChatMessageReq(Net::ChatMessageReq cma);
	void sendPlayerConfigReq(Net::PlayerConfigReq pca, std::function<void(GameObject*)>);
private:
	void handleUdpConnection();
	void deleteAcknowledgement(Net::Timestamp t);
	void checkAcknowledgements();
	void receiveChatMessageAck(Net::GamePacket packet);
	void receivePlayerConfigAck(Net::GamePacket packet);
	void receiveGameState(Net::GamePacket packet);
	void receiveGameChat(Net::GamePacket packet);

	void handleTimeSync();

	Controller& m_c;
	ClientGameController& m_gc;

	sf::Clock m_timeSyncClock;
	sf::Time m_timeT; // mesures time since NetController started
	sf::Time m_timeSyncT = TIMESYNC_START_DT; // start with sync time to directly send first packet
	sf::Time m_timeSyncDt = TIMESYNC_START_DT;
	Net::TimeSyncPeer m_timeSyncPeer;

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
	std::map<Net::Timestamp, std::function<void(GameObject*)>> m_playerConfigReqCbs;
	std::map<Net::Timestamp, Net::PlayerConfigReq> m_playerConfigReqs;

	std::map<Net::Timestamp, Net::PlayerInput> m_playerInputs;
	std::map<Net::Timestamp, Net::GameState> m_gameStates;
	std::map<Net::Timestamp, Net::ChatMessageReq> m_gameChat;
};
#endif
