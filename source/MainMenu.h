#ifndef SWONE_MAINMENU_H
#define SWONE_MAINMENU_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Color.hpp>
#include <graphics/ParticleSystem.h>
#include <Handleable.h>
#include <string>
#include <Controller.h>
#include <Settings.h>
#include <SFML/Graphics/Drawable.hpp>


namespace Menupoint {
enum { LOKAL, ONLINE, SETTINGS, EXIT, FIRST = LOKAL, LAST = EXIT};
}

class MainMenu: public sf::Drawable,public Handleable {
private:

    int TEXT_MARGIN_LEFT = 30;
    int TEXT_MARGIN_TOP = 50;
    int TEXT_HEADER_SIZE = 50;
    int TEXT_MENUPOINT_SIZE = 30;
    int TEXT_SELECTED_SIZE = 40;

    sf::Text m_header;
    sf::Text m_lokal;
    sf::Text m_online;
    sf::Text m_settings;
    sf::Text m_exit;

    //ParticleSystem m_ps;
    Controller & m_controller;

    unsigned char m_selectedMenupoint;


    void changeFont(sf::Font &);
    void changeSelectedMenupoint();
    void selectedStyle(sf::Text& t);
    void deselectedStyle(sf::Text& t);

    void m_lokalPressed();
    void m_onlinePressed();
    void m_settingsPressed();
    void m_exitPressed();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:

    void setSelectedMenupoint(unsigned int smp);
    void update(sf::Time ellapsed);
    void setFont(sf::Font & font);
    void event(sf::Event& e);
    MainMenu(Controller& c);
    ~MainMenu();
};

#endif
