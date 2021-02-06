#ifndef SWONE_CLIENT_GAME_MAP_DRAWING_H
#define SWONE_CLIENT_GAME_MAP_DRAWING_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include "Updateable.h"

#include "game/Map.h"

class MapDrawing : public sf::Drawable, public sf::Transformable {
public:
	MapDrawing(const Map& map);
	virtual ~MapDrawing();

	void createMapImage();

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
	sf::Vector2i getTileTexturePosition(MapTile t);
	const Map& m_map;

	size_t m_imgWidth = 0;
	size_t m_imgHeight = 0;

	sf::Image m_mapImage;//Img with map tiles
	sf::Texture m_texture;
	sf::Sprite m_sprite;

	bool m_mapDrawable = false;
};

#endif
