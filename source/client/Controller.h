#ifndef SWONE_CLIENT_CONTROLLER_H
#define SWONE_CLIENT_CONTROLLER_H

#include <string>
#include <mutex>
#include <vector>
#include <stack>
#include <SFML/Graphics.hpp>

#include "game/Net.h"
#include "util/Log.h"
#include "client/Settings.h"
#include "client/game/NetController.h"
#include "game/GameController.h"

class Controller {
public:
	enum State : char { MainMenu, LocalMenu, OnlineMenu, SettingsMenu, Game };

	Controller(Settings& settings, sf::RenderWindow& w);
	virtual ~Controller();

	void popState();
	void pushState(State i);
	State getState();

	Settings& getSettings();
	sf::RenderWindow& getWindow();
	GameController& getGameController();
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
	GameController m_gc;
	NetController m_nc;

	bool m_run = false;

	std::stack<State> m_stateStack;
};

#endif
