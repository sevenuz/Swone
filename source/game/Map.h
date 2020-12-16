#ifndef MAP_H
#define MAP_H

#include <string>
#include <stdexcept>
#include <memory>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "Controller.h"
#include "Handleable.h"
#include "util/Log.h"

#include "physics/Body.h"
#include "physics/Shape.h"

#define GRAVITY 25

#define TILE_SIZE 1.0f

enum MapTile : char { SHARP = 0, UNDERSCORE = 1, W = 2, SPACE = 3, DEFAULT = SPACE };

struct Tile;

class Map : public Handleable
{
public:
	static const size_t TILE_WIDTH = 64;
	static const size_t TILE_HEIGHT = 64;

	static float toMapPixelX(float x);
	static float toMapPixelY(float y);
	static sf::Vector2f toMapPixel(sf::Vector2f v);

	Map(Controller& c);
	virtual ~Map();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);

	void setName(std::string);
	std::string getName();

	void setTileTexturePath(std::string);
	std::string getTileTexturePath();

	void setGravity(float f);
	float getGravity();

	sf::Sprite& getSprite();

	void scaleToFit();
	void scaleToFit(size_t w, size_t h);

	void setWidth(size_t);
	void setHeight(size_t);
	void setBorder(MapTile);
	size_t getWidth();
	size_t getHeight();
	size_t getImageWidth();
	size_t getImageHeight();

	float getScale();

	MapTile getBorder();
	const Tile& getTile(int h, int w);
	void setMapDataValue(size_t h, size_t w, MapTile v);
	std::map<int, std::map<int, Tile*>>& getMapData();

	void createMapImage();
	static sf::Vector2i getTileTexturePosition(MapTile t);
protected:
private:
	Controller& m_controller;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	std::string m_name = "";
	float m_gravity = GRAVITY;

	size_t m_width = 0;
	size_t m_height = 0;

	size_t m_mapDataSize = 0;

	size_t m_imgWidth = 0;
	size_t m_imgHeight = 0;

	float m_scale = 1;

	std::map<int, std::map<int, Tile*>> m_mapData;//pointer (2d-array) to Map Infos
	MapTile m_border;

	std::string m_tileTexturePath;
	sf::Image m_tileTexture;//Img with all tiles
	sf::Image m_mapImage;//Img with map tiles
	sf::Texture m_texture;
	sf::Sprite m_sprite;

	bool m_mapDrawable = false;
};

struct Tile : public ph::Body::Callback {
	static constexpr const char* MAP_TILE_TYPE = "MapTile";

	ph::PolygonShape shape;
	ph::Body* body;
	sf::Vector2i pos;
	MapTile type;
	//sf::Vertex m_vertices[4];

	Tile(sf::Vector2i pos, MapTile type);

	//virtual void onCollision(ph::Manifold* manifold) override;
	const std::string getType() const override;
	static Tile* castBodyCallback(ph::Body::Callback* c);

	bool isPassable() const;
};

#endif // MAP_H
