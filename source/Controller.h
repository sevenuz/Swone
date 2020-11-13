#ifndef SWONE_CONTROLLER_H
#define SWONE_CONTROLLER_H

#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

#include "util/Log.h"
#include "Settings.h"

// Prefix in front of First and Last is necessary to avoid name clash of the both enums
enum ActiveWindow : char { MAINMENU, SETTINGS, GAME, AW_FIRST = MAINMENU, AW_LAST = GAME };

enum ActiveGameWindow : char { MAPSELECTION, INGAME, AGW_FIRST = MAPSELECTION, AGW_LAST = INGAME };

/*
//connection listener declare:
enum close_reason {
	close_reason_normal,
	close_reason_drop
};
typedef std::function<void(void)> con_listener;

typedef std::function<void(close_reason const& reason)> close_listener;
*/
class Controller {
public:
	Controller(sf::RenderWindow& w);
	virtual ~Controller();

	Settings settings;

	void setActiveWindow(ActiveWindow i);
	ActiveWindow getActiveWindow();

	void setActiveGameWindow(ActiveGameWindow i);
	ActiveGameWindow getActiveGameWindow();

	sf::Vector2f getScale();
	void setScale(sf::Vector2f scale);

	sf::RenderWindow& getWindow();

	void setView(sf::View view);
	void setDefaultView();
protected:
private:
	sf::RenderWindow& m_window;

	void iniSettings();
	ActiveWindow m_activeWindow = ActiveWindow::AW_FIRST;
	ActiveGameWindow m_activeGameWindow = ActiveGameWindow::AGW_FIRST;

	sf::Vector2f m_scale = sf::Vector2f(1, 1);
};

#endif // SWONE_CONTROLLER_H
