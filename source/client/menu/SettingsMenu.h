#ifndef SWONE_CLIENT_SETTINGSMENU_H
#define SWONE_CLIENT_SETTINGSMENU_H

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <string>
#include <sstream>

#include "graphics/ParticleSystem.h"
#include "Handleable.h"
#include "client/Settings.h"
#include "client/Controller.h"
#include "util/Log.h"
#include "util/Helper.h"

class SettingsMenu : public Handleable {
private:
	ParticleSystem m_ps;
	Controller& m_controller;

	sf::Text m_header;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
	void drawImgui();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);

	SettingsMenu(Controller& c);
	~SettingsMenu();
};

#endif
