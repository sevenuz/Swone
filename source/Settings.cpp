#include <Settings.h>

Settings::Settings()
{
	Reader r(SETTINGS_FILE);
	r.forEach([](std::string p, StringPair s){
		std::cout << p << " " << s.first << "=" << s.second << std::endl;
	});
	setFontSource(m_font_src);
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

bool Settings::getVerticalSyncEnabled()
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

std::string Settings::getFontSource()
{
	return m_font_src;
}

void Settings::setFontSource(std::string s)
{
	m_font_src = s;
	if (!m_font.loadFromFile(m_font_src)) {
		throw std::invalid_argument("font not found");
	}
}

sf::Font& Settings::getFont()
{
	return m_font;
}

sf::Color Settings::getClearingColor()
{
	return m_clearing_color;
}

void Settings::setClearingColor(sf::Color c)
{
	m_clearing_color = c;
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
