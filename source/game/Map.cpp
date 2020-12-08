#include "Map.h"

Map::Map(Controller& c) : m_controller(c) // @suppress("Class members should be properly initialized")
{}

Map::~Map() {
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

void Map::setGravity(float f)
{
	m_gravity = f;
}

float Map::getGravity()
{
	return m_gravity;
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

float Map::getScale() {
	return m_scale;
}

const Tile& Map::getTile(int h, int w) {
	if (!m_mapData[h].count(w)) {
		// add border tile to map if coordinate not exists
		setMapDataValue(h, w, m_border);
	}
	return *m_mapData.at(h).at(w);
}

void Map::setMapDataValue(size_t h, size_t w, MapTile t) {
	m_mapData[h][w] = new Tile(sf::Vector2i(w, h), t);
}

std::map<int, std::map<int, Tile*>>& Map::getMapData()
{
	return m_mapData;
}

void Map::scaleToFit() {
	scaleToFit(m_controller.getSettings().getWidth(), m_controller.getSettings().getHeight());
}

void Map::scaleToFit(size_t w, size_t h) {
	float scaleW = (float)(w) / (float)(m_imgWidth);
	float scaleH = (float)(h) / (float)(m_imgHeight);

	m_scale = (scaleW > scaleH) ? scaleH : scaleW;
	m_controller.setScale(sf::Vector2f(m_scale, m_scale));
}

void Map::createMapImage() {
	if (!m_mapTiles.loadFromFile("../res/tile.png")) {
		throw std::invalid_argument("tile.png not found!");
	}
	m_imgWidth = Map::TILE_WIDTH * m_width;
	m_imgHeight = Map::TILE_HEIGHT * m_height;
	m_mapImage.create(m_imgWidth, m_imgHeight, sf::Color::Black);
	for (size_t r = 0; r < m_width; r++)
		for (size_t c = 0; c < m_height; c++)
			m_mapImage.copy(m_mapTiles, c * Map::TILE_WIDTH, r * Map::TILE_HEIGHT,
				sf::IntRect(getTile(r, c).type * Map::TILE_WIDTH, 0,
					Map::TILE_WIDTH, Map::TILE_HEIGHT), true);
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
			Log::ger().log("color switch");

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

const std::string Tile::getType() const
{
	return MAP_TILE_TYPE;
}

Tile* Tile::castBodyCallback(ph::Body::Callback* c)
{
	if(c->getType() == MAP_TILE_TYPE)
		return static_cast<Tile*>(c);
	else
		return NULL;
}

bool Tile::isPassable() const {
	switch (type) {
		case MapTile::SHARP:
		case MapTile::UNDERSCORE:
		case MapTile::W:
			return false;
		default:
			return true;
	}
}
