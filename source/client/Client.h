#ifndef SWONE_CLIENT_CLIENT_H
#define SWONE_CLIENT_CLIENT_H

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <string>
#include <thread>

#include "client/menu/MainMenu.h"
#include "client/menu/LocalMenu.h"
#include "client/menu/OnlineMenu.h"
#include "client/menu/SettingsMenu.h"
#include "client/game/GameWindow.h"
#include "client/Controller.h"
#include "client/Settings.h"
#include "graphics/ParticleSystem.h"
#include "util/reader/Reader.h"
#include "util/Log.h"
#include "server/Server.h"

class Client {
public:
	Client();
	virtual ~Client();

	int start();
	void stop();
private:
	void handleAllEvents(sf::Event& event);
	void drawLog();
	void renderObjectSelector();
	void drawObjectViewer();
	void startMainLoop();
	void initLogger();

	Settings settings;
	sf::RenderWindow window;
	Controller controller;
	MainMenu menu;
	LocalMenu localMenu;
	OnlineMenu onlineMenu;
	SettingsMenu settingsMenu;
	GameWindow gameWindow;
	Server server;

	sf::Clock game_clock;

	bool object_viewer_activated = false;
	bool demo_window_activated = false;
};

#endif // SWONE_CLIENT_CLIENT_H

