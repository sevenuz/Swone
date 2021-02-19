#include "Map.h"

Map::Map() {}

Map::~Map() {}

float Map::toMapDimension(float x)
{
	return x * Map::TILE_WIDTH;
}

float Map::toMapPixelX(float x) {
	return (x-TILE_CORRECTION) * Map::TILE_WIDTH;
}

float Map::toMapPixelY(float y) {
	return (y-TILE_CORRECTION) * Map::TILE_HEIGHT;
}

sf::Vector2f Map::toMapPixel(sf::Vector2f v)
{
	return sf::Vector2f(toMapPixelX(v.x), toMapPixelY(v.y));
}

void Map::setName(std::string n) {
	m_name = n;
}
std::string Map::getName() {
	return m_name;
}

void Map::setTileTexturePath(std::string n) {
	m_tileTexturePath = n;
}
std::string Map::getTileTexturePath() const {
	return m_tileTexturePath;
}

void Map::setTileTextureName(std::string n) {
	m_tileTextureName = n;
}
std::string Map::getTileTextureName() {
	return m_tileTextureName;
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

size_t Map::getWidth() const {
	return m_width;
}

size_t Map::getHeight() const {
	return m_height;
}

const size_t Map::getImageWidth() const {
	return m_imgWidth;
}

const size_t Map::getImageHeight() const {
	return m_imgHeight;
}

MapTile Map::getBorder() {
	return m_border;
}

const Tile& Map::getTile(int h, int w) const {
	return *m_mapData.at(h).at(w);
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

Tile::Tile(sf::Vector2i pos, MapTile t)
	 : pos(pos), type(t)
	{
		if(type == MapTile::UNDERSCORE) {
			shape.SetBox(TILE_SIZE/2, TILE_SIZE/4);
			body = new ph::Body(ph::Body::Config{.x = ((float)pos.x), .y = ((float)pos.y+TILE_CORRECTION/2)}, &shape, this);
		} else {
			shape.SetBox(TILE_SIZE/2, TILE_SIZE/2);
			body = new ph::Body(ph::Body::Config{.x = ((float)pos.x), .y = ((float)pos.y)}, &shape, this);
		}
		body->SetSolid();
		/*
		m_vertices[0].position = Map::toMapPixel(sf::Vector2f(pos.x, pos.y));
		m_vertices[1].position = Map::toMapPixel(sf::Vector2f(pos.x, pos.y + TILE_SIZE));
		m_vertices[2].position = Map::toMapPixel(sf::Vector2f(pos.x + TILE_SIZE, pos.y + TILE_SIZE));
		m_vertices[3].position = Map::toMapPixel(sf::Vector2f(pos.x + TILE_SIZE, pos.y));

		sf::Vector2i ttp = Map::getTileTexturePosition(type);
		m_vertices[0].texCoords = sf::Vector2f(ttp.x, ttp.y);
		m_vertices[1].texCoords = sf::Vector2f(ttp.x, ttp.y + Map::TILE_HEIGHT);
		m_vertices[2].texCoords = sf::Vector2f(ttp.x + Map::TILE_WIDTH, ttp.y + Map::TILE_HEIGHT);
		m_vertices[3].texCoords = sf::Vector2f(ttp.x + Map::TILE_WIDTH, ttp.y);
		*/
}

const std::string Tile::getCallbackType() const
{
	return MAP_TILE_TYPE;
}

Tile* Tile::castBodyCallback(ph::Body::Callback* c)
{
	if(c->getCallbackType() == MAP_TILE_TYPE)
		return static_cast<Tile*>(c);
	else
		return nullptr;
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
