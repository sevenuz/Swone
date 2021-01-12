#ifndef SWONE_CLIENT_ONLINEMENU_H
#define SWONE_CLIENT_ONLINEMENU_H

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Color.hpp>

#include <stack>
#include <vector>
#include <string>
#include <sstream>
#include <thread>

#include "graphics/ParticleSystem.h"
#include "game/GameController.h"
#include "game/GameReader.h"
#include "game/Scenery.h"
#include "game/Net.h"
#include "Handleable.h"
#include "client/GameWindow.h"
#include "client/Settings.h"
#include "client/Controller.h"
#include "util/Log.h"
#include "util/Helper.h"

class OnlineMenu : public Handleable {
private:
	ParticleSystem m_ps;
	Controller& m_controller;

	sf::Text m_header;
	std::string m_selectedScenery = "";
	std::string m_lobbyName = "Swone's Fightclub";
	std::string m_lobbyPassword = "";
	bool m_lobbyPrivate = false;

	std::stack<std::string> m_modalMessageStack;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawJoinWindow();
	void drawCreateWindow();

	void createLobby();
public:
	void sendLobbyRequest(Net::CreateLobbyReq clr);

	void drawImgui();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);

	OnlineMenu(Controller& c);
	~OnlineMenu();
};

#endif
