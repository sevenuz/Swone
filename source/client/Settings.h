#ifndef SWONE_CLIENT_SETTINGS_H
#define SWONE_CLIENT_SETTINGS_H

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include "util/reader/Reader.h"
#include "util/Log.h"
#include "util/Helper.h"

#define SETTINGS_FILE "../res/settings.ini"

// settings property names
#define SETTINGS_WIDTH "width"
#define SETTINGS_HEIGHT "height"
#define SETTINGS_BITS_PER_PIXEL "bits_per_pixel"
#define SETTINGS_VERTICAL_SYNC_ENABLED "vertical_sync_enabled"
#define SETTINGS_RESOURCE_DIRECTORY "resource_directory"
#define SETTINGS_FONT_SOURCE "font_source"
#define SETTINGS_CLEARING_COLOR "clearing_color"
#define SETTINGS_SERVER "server"
#define SETTINGS_PORT "port"

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

	bool isVerticalSyncEnabled();
	void setVerticalSyncEnabled(bool v);

	std::string getResourceDirectory();
	void setResourceDirectory(std::string s);

	std::string getFontSource();
	void setFontSource(std::string s);

	sf::Font& getFont();
	void loadFont();

	sf::Color getClearingColor();
	void setClearingColor(sf::Color c);

	std::string getServer();
	void setServer(std::string v);

	unsigned short getPort();
	void setPort(unsigned short v);

	sf::IpAddress getServerIpAddress();
	std::string getServerAndPort();

	bool isChanged();
	void setChanged(bool v);

	void writeSettings();

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

	std::string m_resource_directory = "../res/";

	std::string m_font_src = "../res/Roboto-Medium.ttf";
	sf::Font m_font;
	sf::Color m_clearing_color = sf::Color::Black;

	std::string m_server = "localhost";
	unsigned short m_port = 61007;

	// indicates if the settings changed compared to settings.ini
	bool m_changed = false;
};

#endif

