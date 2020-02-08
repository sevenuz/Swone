#ifndef SWONE_CONTROLLER_H
#define SWONE_CONTROLLER_H

#include <iostream>
#include <Settings.h>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

namespace ActiveWindow {
	enum { MAINMENU, SETTINGS, GAME, FIRST = MAINMENU, LAST = GAME};
}

namespace ActiveGameWindow {
	enum { MAPSELECTION, GAME, FIRST = MAPSELECTION, LAST = GAME};
}
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

    void setActiveWindow(unsigned int i);
    unsigned int getActiveWindow();

    void setActiveGameWindow(unsigned int i);
    unsigned int getActiveGameWindow();

    sf::Vector2f getScale();
    void setScale(sf::Vector2f scale);

    sf::RenderWindow& getWindow();

    //Log
    bool m_log_closed;

    void toggleLogWindow();
    bool isLogClosed();
    void pushLogMsg(std::string msg, std::string type = "log");
    void pushLogMsg(float msg, std::string type = "log");
    void replaceLastLogMsg(std::string msg, std::string type = "log");
    void clearLog();
    std::string& getLastLogMsg();
    std::vector<std::string>* getMsgList();
    std::vector<std::string>* getMsgTypeList();

    void setView(sf::View view);
    void setDefaultView();
protected:

private:

    sf::RenderWindow& m_window;

    void iniSettings();
    unsigned int m_activeWindow = ActiveWindow::FIRST;
    unsigned int m_activeGameWindow = ActiveGameWindow::FIRST;

    sf::Vector2f m_scale = sf::Vector2f(1,1);

    //Log
    char m_log_input;
    std::vector<std::string> m_log_msgList;
    std::vector<std::string> m_log_msgTypeList;
};

#endif // SWONE_CONTROLLER_H
