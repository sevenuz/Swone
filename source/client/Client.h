#ifndef SWONE_CLIENT_CLIENT_H
#define SWONE_CLIENT_CLIENT_H

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <string>
#include <thread>

#include "client/MainMenu.h"
#include "client/GameMenu.h"
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
	void initSocket();
	void initLogger();

	Settings settings;
	sf::RenderWindow window;
	Controller controller;
	MainMenu menu;
	GameMenu gameMenu;
	Server server;

	sf::Clock game_clock;

	sf::UdpSocket socket;

	bool key_strg_pressed = false;
	bool key_l_pressed = false;
	bool object_viewer_activated = false;
	bool demo_window_activated = false;
};

#endif // SWONE_CLIENT_CLIENT_H

