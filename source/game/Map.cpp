#include "Map.h"

Map::Map() {}

Map::~Map() {}

float Map::toMapPixelX(float x) {
	return x * Map::TILE_WIDTH;
}

float Map::toMapPixelY(float y) {
	return y * Map::TILE_HEIGHT;
}

sf::Vector2f Map::toMapPixel(sf::Vector2f v)
{
	return sf::Vector2f(toMapPixelX(v.x), toMapPixelY(v.y));
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

void Map::setTileTexturePath(std::string n) {
	m_tileTexturePath = n;
}
std::string Map::getTileTexturePath() {
	return m_tileTexturePath;
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

void Map::createMapImage() {
	if(m_tileTexturePath.empty())
		throw std::invalid_argument("Map needs a texture!");
	const sf::Image tileTexture = *Helper::loadImage(m_tileTexturePath);

	m_imgWidth = Map::TILE_WIDTH * m_width;
	m_imgHeight = Map::TILE_HEIGHT * m_height;
	m_mapImage.create(m_imgWidth, m_imgHeight, sf::Color::Black);
	for (size_t r = 0; r < m_width; r++)
		for (size_t c = 0; c < m_height; c++) {
			sf::Vector2i ttp = Map::getTileTexturePosition(getTile(r, c).type);
			m_mapImage.copy(tileTexture, c * Map::TILE_WIDTH, r * Map::TILE_HEIGHT,
				sf::IntRect(ttp.x, ttp.y,
					Map::TILE_WIDTH, Map::TILE_HEIGHT), true);
		}
	if (!m_texture.loadFromImage(m_mapImage)) {
		throw std::invalid_argument("error: convert img to texture.");
	} else {
		m_sprite.setTexture(m_texture);
		m_sprite.setPosition(sf::Vector2f(0, 0));

		m_mapDrawable = true;
	}
}

sf::Vector2i Map::getTileTexturePosition(MapTile t)
{
	// TODO Map also vertical in textures
	return sf::Vector2i(t * Map::TILE_WIDTH, 0);
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
		/*
		// TODO try Map Vertex Rendering with sf::RenderTexture
		// Render Map from Vertexes
		states.texture = &m_mapTiles;
		for (size_t r = 0; r < m_width; r++)
			for (size_t c = 0; c < m_height; c++)
				target.draw(m_mapData.at(c).at(r)->m_vertices, 4, sf::Quads, states);
		*/
	}
};

Tile::Tile(sf::Vector2i pos, MapTile t)
	 : pos(pos), type(t)
	{
		if(type == MapTile::UNDERSCORE) {
			shape.SetBox(TILE_SIZE/2, TILE_SIZE/4);
			body = new ph::Body(ph::Body::Config{&shape, ((float)pos.x)+TILE_SIZE/2, ((float)pos.y)+TILE_SIZE/4, this});
		} else {
			shape.SetBox(TILE_SIZE/2, TILE_SIZE/2);
			body = new ph::Body(ph::Body::Config{&shape, ((float)pos.x)+TILE_SIZE/2, ((float)pos.y)+TILE_SIZE/2, this});
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
