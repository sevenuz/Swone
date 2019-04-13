#include "Map.h"

Map::Map(Controller & c) : m_controller(c) // @suppress("Class members should be properly initialized")
{
}

Map::~Map() {
	//dtor
	delete[] m_mapData;
}

sf::Sprite & Map::getSprite(){
	return m_sprite;
}

void Map::setName(std::string n) {
	m_name = n;
}
std::string Map::getName() {
	return m_name;
}

void Map::setWidth(unsigned int w) {
	m_width = w;
}

void Map::setHeight(unsigned int h) {
	m_height = h;
}

void Map::setBorder(std::string b) {
	m_border = b;
}

unsigned int Map::getWidth() {
	return m_width;
}

unsigned int Map::getHeight() {
	return m_height;
}

unsigned int Map::getImageWidth() {
	return m_imgWidth;
}

unsigned int Map::getImageHeight() {
	return m_imgHeight;
}

std::string Map::getBorder() {
	return m_border;
}

unsigned int & Map::getMapData() {
	return *m_mapData;
}

float Map::getScale(){
	return m_scale;
}

unsigned int Map::getMapDataValue(unsigned int h, unsigned int w) {
	return m_mapData[h * m_width + w];
}

void Map::setMapDataValue(unsigned int h, unsigned int w, unsigned int v) {
	m_mapData[h * m_width + w] = v;
}

void Map::scaleToFit(){
	scaleToFit(m_controller.settings.WIDTH, m_controller.settings.HEIGHT);
}

void Map::scaleToFit(unsigned int w, unsigned int h){
	float scaleW = (float)(w) / (float)(m_imgWidth);
	float scaleH = (float)(h) / (float)(m_imgHeight);
	if (scaleW > scaleH) {
		m_scale = scaleH;
		m_sprite.setScale(sf::Vector2f(scaleH,scaleH));
	} else {
		m_scale = scaleW;
		m_sprite.setScale(sf::Vector2f(scaleW,scaleW));
	}
}

void Map::iniMapData() {
	if (m_width > 0 && m_height > 0) {
		m_mapData = new unsigned int[m_height * m_width];
	} else {
		throw std::invalid_argument("width or height are not set");
	}
}

void Map::createMapImage() {
	if (!m_mapTiles.loadFromFile("../res/tile.png")) {
		throw std::invalid_argument("tile.png not found!");
	}
	m_imgWidth = Map::TILE_WIDTH * m_width;
	m_imgHeight = Map::TILE_HEIGHT * m_height;
	m_mapImage.create(m_imgWidth, m_imgHeight, sf::Color::Black);
	int l = 0; //lines
	int c = -1; //columns
	for (unsigned int i = 0; i < m_width * m_height; i++) {
		if (i < m_width * (l + 1)) {
			c++;
		} else {
			c = 0;
			l++;
		}
		m_mapImage.copy(m_mapTiles, c * Map::TILE_WIDTH, l * Map::TILE_HEIGHT,
				sf::IntRect(getMapDataValue(l, c) * Map::TILE_WIDTH, 0,
						Map::TILE_WIDTH, Map::TILE_HEIGHT), true);
	}
	if (!m_texture.loadFromImage(m_mapImage)) {
		//error
	} else {
		m_sprite.setTexture(m_texture);
		m_sprite.setPosition(sf::Vector2f(0, 0));
		scaleToFit();

		m_mapDrawable = true;
	}
}

void Map::event(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::C) {
			m_controller.pushLogMsg("color switch");

			m_sprite.setColor(sf::Color(0, 255, 0, 128));
		}
	}
}

void Map::update(sf::Time ellapsed) {
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;
	if(m_mapDrawable)
	{
		target.draw(m_sprite, states);
	}
};
