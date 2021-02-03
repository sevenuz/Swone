#include "game/GameReader.h"
#include <client/Settings.h>

Settings::Settings()
{
	try {
		Log::ger().log("-----Settings-----");
		read(Helper::getSavePath(SETTINGS_FILE));
		Log::ger().log("------------------");
	} catch(const std::invalid_argument& ia) {
		Log::ger().log(ia.what(), Log::Label::Error);
		std::filesystem::create_directories(Helper::getSavePath());
		writeSettings();
		Log::ger().log("Write Default-Settings.");
		read(Helper::getSavePath(SETTINGS_FILE));
	}
	// set flag to false again to indicate that the settings in the file
	// are the settings here, the flag will changed on next changes again
	setChanged(false);
	loadFont();
}

Settings::~Settings()
{
	// TODO fix: write settings always
	if(isChanged())
		writeSettings();
}

void Settings::read(const std::string& s)
{
	Reader r(s);
	auto& setupMap = r.getParagraphMap();
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH)){
		auto& global = setupMap[Reader::DEFAULT_PARAGRAPH];
		if(global.count(SETTINGS_WIDTH))
			setWidth(Helper::toLong(global[SETTINGS_WIDTH]));
		if(global.count(SETTINGS_HEIGHT))
			setHeight(Helper::toLong(global[SETTINGS_HEIGHT]));
		if(global.count(SETTINGS_BITS_PER_PIXEL))
			setBitsPerPixel(Helper::toLong(global[SETTINGS_BITS_PER_PIXEL]));
		if(global.count(SETTINGS_VERTICAL_SYNC_ENABLED))
			setVerticalSyncEnabled(Helper::toBool(global[SETTINGS_VERTICAL_SYNC_ENABLED]));
		if(global.count(SETTINGS_RESOURCE_DIRECTORY))
			setResourceDirectory(global[SETTINGS_RESOURCE_DIRECTORY]);
		if(global.count(SETTINGS_DOWNLOAD_DIRECTORY))
			setDownloadDirectory(global[SETTINGS_DOWNLOAD_DIRECTORY]);
		if(global.count(SETTINGS_FONT_SOURCE))
			setFontSource(global[SETTINGS_FONT_SOURCE]);
		if(global.count(SETTINGS_CLEARING_COLOR))
			setClearingColor(Helper::toColor(global[SETTINGS_CLEARING_COLOR]));
	}

	for(auto& m : setupMap) {
		if(m.first.rfind(SETTINGS_KEYBINDING_PROFILE_PREFIX, 0) == 0) {
			m_keybindingProfiles[m.first.substr(m.first.find_first_of("_")+1)] = Keybinding{
				(sf::Keyboard::Key)Helper::toInt(m.second[SETTINGS_KEYBINDING_LEFT]),
				(sf::Keyboard::Key)Helper::toInt(m.second[SETTINGS_KEYBINDING_RIGHT]),
				(sf::Keyboard::Key)Helper::toInt(m.second[SETTINGS_KEYBINDING_UP]),
				(sf::Keyboard::Key)Helper::toInt(m.second[SETTINGS_KEYBINDING_DOWN]),
				(sf::Keyboard::Key)Helper::toInt(m.second[SETTINGS_KEYBINDING_ACTION1]),
				(sf::Keyboard::Key)Helper::toInt(m.second[SETTINGS_KEYBINDING_ACTION2]),
				(sf::Keyboard::Key)Helper::toInt(m.second[SETTINGS_KEYBINDING_ACTION3]),
				(sf::Keyboard::Key)Helper::toInt(m.second[SETTINGS_KEYBINDING_ACTION4])
			};
		}
	}
	if(m_keybindingProfiles.size() == 0) {
		//add default profile
		m_keybindingProfiles["Default"] = {
			sf::Keyboard::A,
			sf::Keyboard::D,
			sf::Keyboard::W,
			sf::Keyboard::S,
			sf::Keyboard::Left,
			sf::Keyboard::Down,
			sf::Keyboard::Right,
			sf::Keyboard::Up,
		};
	}

	r.forEach([&](std::string p, std::string k, std::string v){
		Log::ger().log(k + " = " + v);
	});
}

void Settings::addKeybinding(std::string name, Keybinding k)
{
	m_keybindingProfiles[name] = k;
}

StringMapMap Settings::serializeKeybindings()
{
	StringMapMap k;
	for(auto& p : m_keybindingProfiles) {
		k[std::string(SETTINGS_KEYBINDING_PROFILE_PREFIX) + p.first] = {
			StringPair(SETTINGS_KEYBINDING_LEFT, std::to_string(p.second.left)),
			StringPair(SETTINGS_KEYBINDING_RIGHT, std::to_string(p.second.right)),
			StringPair(SETTINGS_KEYBINDING_UP, std::to_string(p.second.up)),
			StringPair(SETTINGS_KEYBINDING_DOWN, std::to_string(p.second.down)),
			StringPair(SETTINGS_KEYBINDING_ACTION1, std::to_string(p.second.action1)),
			StringPair(SETTINGS_KEYBINDING_ACTION2, std::to_string(p.second.action2)),
			StringPair(SETTINGS_KEYBINDING_ACTION3, std::to_string(p.second.action3)),
			StringPair(SETTINGS_KEYBINDING_ACTION4, std::to_string(p.second.action4)),
		};
	}
	return k;
}

void Settings::writeSettings()
{
	StringMapMap sm = {{"global", {
		StringPair(SETTINGS_WIDTH, std::to_string(getWidth())),
		StringPair(SETTINGS_HEIGHT, std::to_string(getHeight())),
		StringPair(SETTINGS_BITS_PER_PIXEL, std::to_string(getBitsPerPixel())),
		StringPair(SETTINGS_VERTICAL_SYNC_ENABLED, isVerticalSyncEnabled()?"true":"false"),
		StringPair(SETTINGS_RESOURCE_DIRECTORY, getResourceDirectory()),
		StringPair(SETTINGS_DOWNLOAD_DIRECTORY, getDownloadDirectory()),
		StringPair(SETTINGS_FONT_SOURCE, getFontSource()),
		StringPair(SETTINGS_CLEARING_COLOR, "Color(" +
			std::to_string(getClearingColor().r) + "," +
			std::to_string(getClearingColor().g) + "," +
			std::to_string(getClearingColor().b) + ")"
		),
		StringPair(SETTINGS_SERVER, getServer()),
		StringPair(SETTINGS_PORT, std::to_string(getPort()))
	}}};
	StringMapMap k = serializeKeybindings();
	sm.insert(k.begin(), k.end());
	Reader::write(Helper::getSavePath(SETTINGS_FILE), sm);
}

size_t Settings::getWidth()
{
	return m_width;
}

void Settings::setWidth(size_t v)
{
	m_width = v;
	setChanged(true);
}

size_t Settings::getHeight()
{
	return m_height;
}

void Settings::setHeight(size_t v)
{
	m_height = v;
	setChanged(true);
}

size_t Settings::getBitsPerPixel()
{
	return m_bits_per_pixel;
}

void Settings::setBitsPerPixel(size_t v)
{
	m_bits_per_pixel = v;
	setChanged(true);
}

bool Settings::isVerticalSyncEnabled()
{
	return m_vertical_sync_enabled;
}

void Settings::setVerticalSyncEnabled(bool v)
{
	m_vertical_sync_enabled = v;
	setChanged(true);
}

std::string Settings::getResourceDirectory()
{
	return m_resource_directory;
}

void Settings::setResourceDirectory(std::string s)
{
	m_resource_directory = Helper::getSavePath(s);
	GameReader::initResDir(m_resource_directory);
	setChanged(true);
}

std::string Settings::getDownloadDirectory()
{
	return m_download_directory;
}

void Settings::setDownloadDirectory(std::string s)
{
	m_download_directory = Helper::getSavePath(s);
	GameReader::initResDir(m_download_directory);
	setChanged(true);
}

std::string Settings::getFontSource()
{
	return m_font_src;
}

void Settings::setFontSource(std::string s)
{
	m_font_src = Helper::getSavePath(s);
	setChanged(true);
}

sf::Font& Settings::getFont()
{
	return m_font;
}

void Settings::loadFont()
{
	if (!m_font.loadFromFile(m_font_src)) {
		Log::ger().log("font not found", Log::Label::Error);
		throw std::invalid_argument("font not found");
	}
}

sf::Color Settings::getClearingColor()
{
	return m_clearing_color;
}

void Settings::setClearingColor(sf::Color c)
{
	m_clearing_color = c;
	setChanged(true);
}

std::string Settings::getServer()
{
	return m_server;
}

void Settings::setServer(std::string v)
{
	m_server = v;
	setChanged(true);
}

void Settings::setPort(unsigned short v)
{
	m_port = v;
	setChanged(true);
}

unsigned short Settings::getPort()
{
	return m_port;
}

sf::IpAddress Settings::getServerIpAddress()
{
	return sf::IpAddress(m_server);
}

std::string Settings::getServerAndPort()
{
	return m_server + ":" + std::to_string(m_port);
}

std::map<std::string, Settings::Keybinding>& Settings::getKeybindings()
{
	return m_keybindingProfiles;
}

Settings::Keybinding Settings::getKeybinding(std::string name)
{
	return m_keybindingProfiles.at(name);
}

bool Settings::isChanged()
{
	return m_changed;
}

void Settings::setChanged(bool v)
{
	m_changed = v;
}

int Settings::toW(float w){
	return (int)(w*m_width);
}

int Settings::toH(float h){
	return (int)(h*m_height);
}

int Settings::toW(int w){
	return toW(((float)w/STANDARD_WIDTH));
}

int Settings::toH(int h){
	return toH(((float)h/STANDARD_HEIGHT));
}

int Settings::toF(int f){
	return toW(f);
}
