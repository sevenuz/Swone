#include <client/Settings.h>

Settings::Settings()
{
	try {
		Log::ger().log("-----Settings-----");
		Reader r(SETTINGS_FILE);
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
			if(m.first.rfind(SETTINGS_CONTROLS_PROFILE_PREFIX, 0) == 0) {
				m_controlProfiles[m.first] = m.second;
			}
		}
		if(m_controlProfiles.size() == 0) {
			//add default profile
			m_controlProfiles[std::string(SETTINGS_CONTROLS_PROFILE_PREFIX) + "Default"] = {
				StringPair(Extension::CONTROLS_LEFT, std::to_string(sf::Keyboard::A)),
				StringPair(Extension::CONTROLS_RIGHT, std::to_string(sf::Keyboard::D)),
				StringPair(Extension::CONTROLS_UP, std::to_string(sf::Keyboard::W)),
				StringPair(Extension::CONTROLS_DOWN, std::to_string(sf::Keyboard::S)),
				StringPair(Extension::CONTROLS_INV1, std::to_string(sf::Keyboard::Left)),
				StringPair(Extension::CONTROLS_INV2, std::to_string(sf::Keyboard::Down)),
				StringPair(Extension::CONTROLS_INV3, std::to_string(sf::Keyboard::Right)),
				StringPair(Extension::CONTROLS_ACTION, std::to_string(sf::Keyboard::Up)),
			};
		}

		r.forEach([&](std::string p, std::string k, std::string v){
			Log::ger().log(k + " = " + v);
		});
		Log::ger().log("------------------");
	} catch(const std::invalid_argument& ia) {
		Log::ger().log(ia.what(), Log::Label::Error);
		writeSettings();
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
	sm.insert(m_controlProfiles.begin(), m_controlProfiles.end());
	Reader::write(SETTINGS_FILE, sm);
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
	m_resource_directory = s;
	// TODO fs::create_directories(m_resource_directory);
	setChanged(true);
}

std::string Settings::getDownloadDirectory()
{
	return m_download_directory;
}

void Settings::setDownloadDirectory(std::string s)
{
	m_download_directory = s;
	// TODO fs::create_directories(m_download_directory);
	// also for obj/ scenery/ map/ texture/
	setChanged(true);
}

std::string Settings::getFontSource()
{
	return m_font_src;
}

void Settings::setFontSource(std::string s)
{
	m_font_src = s;
	setChanged(true);
}

sf::Font& Settings::getFont()
{
	return m_font;
}

void Settings::loadFont()
{
	if (!m_font.loadFromFile(m_font_src)) {
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

StringMapMap& Settings::getControlProfiles()
{
	return m_controlProfiles;
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
