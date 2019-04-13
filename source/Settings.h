#ifndef SWONE_SETTINGS_H
#define SWONE_SETTINGS_H

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <string>

class Settings {
public:
    Settings();
    virtual ~Settings();

    //An den standart werten werden die elemente ausgerichtet
    static const unsigned int STANDART_WIDTH = 640;
    static const unsigned int STANDART_HEIGHT = 360;

    //das ist die eigentliche auflösung
    static const unsigned int WIDTH = 640;
    static const unsigned int HEIGHT = 360;
    static const unsigned int STANDART_BITS_PER_PIXEL = 32;

    static const bool STANDART_VERTICAL_SYNC_ENABLED = true;

    char * mapDir = "../res/map/";

    sf::Font font;

    static int toW(int w);
    static int toH(int h);
    static int toW(float w);
    static int toH(float h);

    static int toF(int f);//font size

    //static sf::Color STANDART_CLEARING_COLOR = sf::Color::Black;

    //static const string STANDART_TEXT_FONT = "C:/Users/teetr/games/FARG_BETA/content/FARG.ttf";

protected:

private:
    std::string fontSrc = "../res/Roboto-Medium.ttf";
};

#endif
