#include <client/game/MapDrawing.h>

MapDrawing::MapDrawing(const Map& map) :
	m_map(map)
{
	createMapImage();
}

MapDrawing::~MapDrawing()
{}

void MapDrawing::createMapImage() {
	if(m_map.getTileTexturePath().empty())
		throw std::invalid_argument("Map needs a texture!");
	const sf::Image tileTexture = *GameReader::loadImage(m_map.getTileTexturePath());

	m_imgWidth = Map::TILE_WIDTH * m_map.getWidth();
	m_imgHeight = Map::TILE_HEIGHT * m_map.getHeight();
	m_mapImage.create(m_imgWidth, m_imgHeight, sf::Color::Black);
	for (size_t r = 0; r < m_map.getWidth(); r++)
		for (size_t c = 0; c < m_map.getHeight(); c++) {
			sf::Vector2i ttp = getTileTexturePosition(m_map.getTile(r, c).type);
			m_mapImage.copy(tileTexture, c * Map::TILE_WIDTH, r * Map::TILE_HEIGHT,
				sf::IntRect(ttp.x, ttp.y,
					Map::TILE_WIDTH, Map::TILE_HEIGHT), true);
		}
	if (!m_texture.loadFromImage(m_mapImage)) {
		throw std::invalid_argument("error: convert img to texture.");
	} else {
		m_sprite.setTexture(m_texture);
		m_sprite.setPosition(sf::Vector2f(TILE_CORRECTION, TILE_CORRECTION));

		m_mapDrawable = true;
	}
}

sf::Vector2i MapDrawing::getTileTexturePosition(MapTile t)
{
       // TODO Map also vertical in textures
       return sf::Vector2i(t * Map::TILE_WIDTH, 0);
}

void MapDrawing::draw(sf::RenderTarget& target, sf::RenderStates states) const {
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

