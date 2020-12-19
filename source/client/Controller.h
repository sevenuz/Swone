#ifndef SWONE_CONTROLLER_H
#define SWONE_CONTROLLER_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

#include "util/Log.h"
#include "client/Settings.h"

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
	Controller(Settings& settings, sf::RenderWindow& w);
	virtual ~Controller();

	void setActiveWindow(ActiveWindow i);
	ActiveWindow getActiveWindow();

	void setActiveGameWindow(ActiveGameWindow i);
	ActiveGameWindow getActiveGameWindow();

	Settings& getSettings();
	sf::RenderWindow& getWindow();

	void setDefaultView();
protected:
private:
	Settings& m_settings;
	sf::RenderWindow& m_window;

	ActiveWindow m_activeWindow = ActiveWindow::AW_FIRST;
	ActiveGameWindow m_activeGameWindow = ActiveGameWindow::AGW_FIRST;
};

#endif // SWONE_CONTROLLER_H
