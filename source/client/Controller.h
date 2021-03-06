#ifndef SWONE_CLIENT_CONTROLLER_H
#define SWONE_CLIENT_CONTROLLER_H

#include <queue>
#include <string>
#include <mutex>
#include <vector>
#include <stack>
#include <SFML/Graphics.hpp>

#include "game/Net.h"
#include "util/Log.h"
#include "client/Settings.h"
#include "client/game/NetController.h"
#include "client/game/ClientGameController.h"

class Controller {
public:
	enum State : char { MainMenu, LocalMenu, OnlineMenu, SettingsMenu, Game };

	Controller(Settings& settings, sf::RenderWindow& w);
	virtual ~Controller();

	void popState();
	void pushState(State i);
	State getState();

	std::queue<std::string>& getModalMessageQueue();
	void pushMessage(std::string s);

	Settings& getSettings();
	sf::RenderWindow& getWindow();
	ClientGameController& getGameController();
	NetController& getNetController();
	void loadGame(Net::JoinLobbyAck jla);

	void setDefaultView();

	bool isRunning();
	void start();
	void stop();

	std::mutex gameMutex;
protected:
private:
	Settings& m_settings;
	sf::RenderWindow& m_window;
	ClientGameController m_gc;
	NetController m_nc;

	bool m_run = false;

	std::stack<State> m_stateStack;
	std::queue<std::string> m_modalMessageQueue;
};

#endif
