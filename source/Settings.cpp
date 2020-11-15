#include <Settings.h>

Settings::Settings()
{
	try {
		Log::ger().log("-----Settings-----");
		Reader r(SETTINGS_FILE);
		r.forEach([&](std::string p, std::string k, std::string v){
			Log::ger().log(k + " = " + v);
			if(k==SETTINGS_WIDTH_NAME)
				setWidth(Helper::toLong(v));
			else if(k==SETTINGS_HEIGHT_NAME)
				setHeight(Helper::toLong(v));
			else if(k==SETTINGS_BITS_PER_PIXEL_NAME)
				setBitsPerPixel(Helper::toLong(v));
			else if(k==SETTINGS_VERTICAL_SYNC_ENABLED_NAME)
				setVerticalSyncEnabled(Helper::toBool(v));
			else if(k==SETTINGS_MAP_DIRECTORY_NAME)
				setMapDirectory(v);
			else if(k==SETTINGS_OBJ_DIRECTORY_NAME)
				setGameObjectDirectory(v);
			else if(k==SETTINGS_FONT_SOURCE_NAME)
				setFontSource(v);
			else if(k==SETTINGS_CLEARING_COLOR_NAME)
				setClearingColor(Helper::toColor(v));
			else
				Log::ger().log(k + " is not a settings option", Log::Label::Warning);
		});
		Log::ger().log("------------------");
	} catch(const std::invalid_argument& ia) {
		Log::ger().log(ia.what(), Log::Label::Error);
		writeSettings();
	}

	loadFont();
}

Settings::~Settings()
{
	//dtor
}

size_t Settings::getWidth()
{
	return m_width;
}

void Settings::setWidth(size_t v)
{
	m_width = v;
}

size_t Settings::getHeight()
{
	return m_height;
}

void Settings::setHeight(size_t v)
{
	m_height = v;
}

size_t Settings::getBitsPerPixel()
{
	return m_bits_per_pixel;
}

void Settings::setBitsPerPixel(size_t v)
{
	m_bits_per_pixel = v;
}

bool Settings::isVerticalSyncEnabled()
{
	return m_vertical_sync_enabled;
}

void Settings::setVerticalSyncEnabled(bool v)
{
	m_vertical_sync_enabled = v;
}

std::string Settings::getMapDirectory()
{
	return m_map_directory;
}

void Settings::setMapDirectory(std::string s)
{
	m_map_directory = s;
}

std::string Settings::getGameObjectDirectory()
{
	return m_obj_directory;
}

void Settings::setGameObjectDirectory(std::string s)
{
	m_obj_directory = s;
}


std::string Settings::getFontSource()
{
	return m_font_src;
}

void Settings::setFontSource(std::string s)
{
	m_font_src = s;
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
}

bool Settings::isChanged()
{
	return m_changed;
}

void Settings::setChanged(bool v)
{
	m_changed = v;
}

void Settings::writeSettings()
{
	Reader::write(SETTINGS_FILE, {{"global", {
		StringPair(SETTINGS_WIDTH_NAME, std::to_string(getWidth())),
		StringPair(SETTINGS_HEIGHT_NAME, std::to_string(getHeight())),
		StringPair(SETTINGS_BITS_PER_PIXEL_NAME, std::to_string(getBitsPerPixel())),
		StringPair(SETTINGS_VERTICAL_SYNC_ENABLED_NAME, isVerticalSyncEnabled()?"true":"false"),
		StringPair(SETTINGS_MAP_DIRECTORY_NAME, getMapDirectory()),
		StringPair(SETTINGS_OBJ_DIRECTORY_NAME, getGameObjectDirectory()),
		StringPair(SETTINGS_FONT_SOURCE_NAME, getFontSource()),
		StringPair(SETTINGS_CLEARING_COLOR_NAME, "Color(" +
			std::to_string(getClearingColor().r) + "," +
			std::to_string(getClearingColor().g) + "," +
			std::to_string(getClearingColor().b) + ")"
		)
	}}});
}

int Settings::toW(float w){
	return (int)(w*STANDARD_WIDTH);
}

int Settings::toH(float h){
	return (int)(h*STANDARD_HEIGHT);
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
