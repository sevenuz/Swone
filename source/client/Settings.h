#ifndef SWONE_CLIENT_SETTINGS_H
#define SWONE_CLIENT_SETTINGS_H

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <vector>
#include "util/reader/Reader.h"
#include "util/Log.h"
#include "util/Helper.h"
#include "game/GameReader.h"
#include "util/Helper.h"

#define SETTINGS_FILE "settings.ini"

// settings property names
#define SETTINGS_WIDTH "width"
#define SETTINGS_HEIGHT "height"
#define SETTINGS_BITS_PER_PIXEL "bits_per_pixel"
#define SETTINGS_VERTICAL_SYNC_ENABLED "vertical_sync_enabled"
#define SETTINGS_RESOURCE_DIRECTORY "resource_directory"
#define SETTINGS_DOWNLOAD_DIRECTORY "download_directory"
#define SETTINGS_FONT_SOURCE "font_source"
#define SETTINGS_CLEARING_COLOR "clearing_color"
#define SETTINGS_SERVER "server"
#define SETTINGS_PORT "port"

#define SETTINGS_KEYBINDING_PROFILE_PREFIX "keybindingProfile_"

#define SETTINGS_KEYBINDING_LEFT "left"
#define SETTINGS_KEYBINDING_RIGHT "right"
#define SETTINGS_KEYBINDING_UP "up"
#define SETTINGS_KEYBINDING_DOWN "down"
#define SETTINGS_KEYBINDING_ACTION1 "action1"
#define SETTINGS_KEYBINDING_ACTION2 "action2"
#define SETTINGS_KEYBINDING_ACTION3 "action3"
#define SETTINGS_KEYBINDING_ACTION4 "action4"

class Settings {
public:
	struct Keybinding {
		sf::Keyboard::Key left = sf::Keyboard::Unknown;
		sf::Keyboard::Key right = sf::Keyboard::Unknown;
		sf::Keyboard::Key up = sf::Keyboard::Unknown;
		sf::Keyboard::Key down = sf::Keyboard::Unknown;
		sf::Keyboard::Key action1 = sf::Keyboard::Unknown;
		sf::Keyboard::Key action2 = sf::Keyboard::Unknown;
		sf::Keyboard::Key action3 = sf::Keyboard::Unknown;
		sf::Keyboard::Key action4 = sf::Keyboard::Unknown;
	};

	Settings();
	virtual ~Settings();
	void read(const std::string& s);

	void writeSettings();

	std::map<std::string, Keybinding>& getKeybindings();
	Keybinding getKeybinding(std::string name);
	void addKeybinding(std::string name, Keybinding k);

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

	std::string getDownloadDirectory();
	void setDownloadDirectory(std::string s);

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
	StringMapMap serializeKeybindings();
	// real resolution
	size_t m_width = 640;
	size_t m_height = 360;

	size_t m_bits_per_pixel = 32;
	bool m_vertical_sync_enabled = true;

	std::string m_resource_directory = "res/";
	std::string m_download_directory = "res/download/";

	std::string m_font_src = "Roboto-Medium.ttf";
	sf::Font m_font;
	sf::Color m_clearing_color = sf::Color::Black;

	std::string m_server = "localhost";
	unsigned short m_port = 61007;

	// key: name of profile, value: Keybinding
	std::map<std::string, Keybinding>  m_keybindingProfiles;

	// indicates if the settings changed compared to settings.ini
	bool m_changed = false;
};

#endif

