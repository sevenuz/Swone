#ifndef SWONE_CLIENT_CONTROLLER_H
#define SWONE_CLIENT_CONTROLLER_H

#include <string>
#include <vector>
#include <stack>
#include <SFML/Graphics.hpp>

#include "game/Net.h"
#include "util/Log.h"
#include "client/Settings.h"
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
	void loadGame(Net::JoinLobbyAck jla);

	void setDefaultView();

	bool isRunning();
	void start();
	void stop();
protected:
private:
	Settings& m_settings;
	sf::RenderWindow& m_window;
	GameController m_gc;

	bool m_run = false;

	std::stack<State> m_stateStack;
};

#endif
