#ifndef SWONE_SETTINGS_H
#define SWONE_SETTINGS_H

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include "util/reader/Reader.h"

#define SETTINGS_FILE "../res/settings.ini"

class Settings {
public:
	Settings();
	virtual ~Settings();

	// standard resolution to calculate relative positions to real resolution
	static const size_t STANDARD_WIDTH = 640;
	static const size_t STANDARD_HEIGHT = 360;

	size_t getWidth();
	void setWidth(size_t v);

	size_t getHeight();
	void setHeight(size_t v);

	size_t getBitsPerPixel();
	void setBitsPerPixel(size_t v);

	bool getVerticalSyncEnabled();
	void setVerticalSyncEnabled(bool v);

	std::string getMapDirectory();
	void setMapDirectory(std::string s);

	std::string getFontSource();
	void setFontSource(std::string s);

	sf::Font& getFont();

	sf::Color getClearingColor();
	void setClearingColor(sf::Color c);

	// gets relative value to calculate absolute position
	int toW(float w);
	int toH(float h);
	// gets position relative to STANDARD RESOLUTION
	// and calculates relative position to call toW(float)
	int toW(int w);
	int toH(int h);
	//font size, same like toW(int)
	int toF(int f);
protected:
private:
	// real resolution
	size_t m_width = 640;
	size_t m_height = 360;

	size_t m_bits_per_pixel = 32;
	bool m_vertical_sync_enabled = true;

	std::string m_map_directory = "../res/map/";

	std::string m_font_src = "../res/Roboto-Medium.ttf";
	sf::Font m_font;
	sf::Color m_clearing_color = sf::Color::Black;

	// indicates if the settings changed compared to settings.ini
	bool m_changed = false;
};

#endif

