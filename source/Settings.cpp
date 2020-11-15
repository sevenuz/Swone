#include <Settings.h>

Settings::Settings()
{
	try {
		Log::ger().log("Settings:");
		Reader r(SETTINGS_FILE);
		r.forEach([&](std::string p, std::string k, std::string v){
			Log::ger().log(k + " = " + v);
			if(k=="width")
				setWidth(Helper::toLong(v));
			else if(k=="height")
				setHeight(Helper::toLong(v));
			else if(k=="bits_per_pixel")
				setBitsPerPixel(Helper::toLong(v));
			else if(k=="vertical_sync_enabled")
				setVerticalSyncEnabled(Helper::toBool(v));
			else if(k=="map_directory")
				setMapDirectory(v);
			else if(k=="obj_directory")
				setGameObjectDirectory(v);
			else if(k=="font_source")
				setFontSource(v);
			else if(k=="clearing_color")
				setClearingColor(Helper::toColor(v));
			else
				Log::ger().log(k + " is not a settings option", Log::Label::Warning);
		});
		Log::ger().log("---------");
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
		StringPair("width", std::to_string(getWidth())),
		StringPair("height", std::to_string(getHeight())),
		StringPair("bits_per_pixel", std::to_string(getBitsPerPixel())),
		StringPair("vertical_sync_enabled", isVerticalSyncEnabled()?"true":"false"),
		StringPair("map_directory", getMapDirectory()),
		StringPair("obj_directory", getGameObjectDirectory()),
		StringPair("font_source", getFontSource()),
		StringPair("clearing_color", "Color(" +
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
