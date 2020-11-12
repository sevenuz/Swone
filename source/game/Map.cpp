#include "Map.h"

Map::Map(Controller& c) : m_controller(c) // @suppress("Class members should be properly initialized")
{
}

Map::~Map() {
	//dtor
	delete[] m_mapData;
}

float Map::toMapPixelX(float x) {
	return x * Map::TILE_WIDTH;
}

float Map::toMapPixelY(float y) {
	return y * Map::TILE_HEIGHT;
}

sf::Sprite& Map::getSprite() {
	return m_sprite;
}

void Map::setName(std::string n) {
	m_name = n;
}
std::string Map::getName() {
	return m_name;
}

void Map::setWidth(size_t w) {
	m_width = w;
}

void Map::setHeight(size_t h) {
	m_height = h;
}

void Map::setBorder(MapTile b) {
	m_border = b;
}

size_t Map::getWidth() {
	return m_width;
}

size_t Map::getHeight() {
	return m_height;
}

size_t Map::getImageWidth() {
	return m_imgWidth;
}

size_t Map::getImageHeight() {
	return m_imgHeight;
}

MapTile Map::getBorder() {
	return m_border;
}

MapTile& Map::getMapData() {
	return *m_mapData;
}

float Map::getScale() {
	return m_scale;
}

MapTile Map::getMapDataValue(size_t h, size_t w) {
	if (h >= m_height || w >= m_height)
		return MapTile::DEFAULT;
	return m_mapData[h * m_width + w];
}

void Map::setMapDataValue(size_t h, size_t w, MapTile v) {
	m_mapData[h * m_width + w] = v;
}

void Map::scaleToFit() {
	scaleToFit(m_controller.settings.WIDTH, m_controller.settings.HEIGHT);
}

void Map::scaleToFit(size_t w, size_t h) {
	float scaleW = (float)(w) / (float)(m_imgWidth);
	float scaleH = (float)(h) / (float)(m_imgHeight);

	m_scale = (scaleW > scaleH) ? scaleH : scaleW;
	m_controller.setScale(sf::Vector2f(m_scale, m_scale));
}

void Map::iniMapData() {
	if (m_width > 0 && m_height > 0) {
		m_mapDataSize = m_height * m_width;
		m_mapData = new MapTile[m_mapDataSize];
	}
	else {
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
	for (size_t i = 0; i < m_mapDataSize; i++) {
		if (i < m_width * (l + 1)) {
			c++;
		}
		else {
			c = 0;
			l++;
		}
		m_mapImage.copy(m_mapTiles, c * Map::TILE_WIDTH, l * Map::TILE_HEIGHT,
			sf::IntRect(getMapDataValue(l, c) * Map::TILE_WIDTH, 0,
				Map::TILE_WIDTH, Map::TILE_HEIGHT), true);
	}
	if (!m_texture.loadFromImage(m_mapImage)) {
		throw std::invalid_argument("error: convert img to texture.");
	}
	else {
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
	if (m_mapDrawable)
	{
		target.draw(m_sprite, states);
	}
};
