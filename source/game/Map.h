#ifndef MAP_H
#define MAP_H

#include <string>
#include <stdexcept>
#include <memory>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "util/Log.h"
#include "util/Helper.h"

#include "game/GameReader.h"

#include "physics/Body.h"
#include "physics/Shape.h"

#define GRAVITY 25

#define TILE_SIZE 1.0f
#define TILE_CORRECTION -0.5f

enum MapTile : char { SHARP = 0, UNDERSCORE = 1, W = 2, SPACE = 3, DEFAULT = SPACE };

struct Tile;
class MapDrawing;

class Map
{
	friend MapDrawing;
public:
	static const size_t TILE_WIDTH = 64;
	static const size_t TILE_HEIGHT = 64;

	static float toMapDimension(float x);
	static float toMapPixelX(float x);
	static float toMapPixelY(float y);
	static sf::Vector2f toMapPixel(sf::Vector2f v);

	Map();
	virtual ~Map();

	void setName(std::string);
	std::string getName();

	void setTileTexturePath(std::string);
	std::string getTileTexturePath() const;

	void setTileTextureName(std::string);
	std::string getTileTextureName();

	void setGravity(float f);
	float getGravity();

	sf::Sprite& getSprite();

	void setWidth(size_t);
	void setHeight(size_t);
	void setBorder(MapTile);
	size_t getWidth() const;
	size_t getHeight() const;
	const size_t getImageWidth() const;
	const size_t getImageHeight() const;

	MapTile getBorder();
	const Tile& getTile(int h, int w) const;
	const Tile& getTile(int h, int w);
	void setMapDataValue(size_t h, size_t w, MapTile v);
	std::map<int, std::map<int, Tile*>>& getMapData();
protected:
private:
	std::string m_name = "";
	float m_gravity = GRAVITY;

	size_t m_width = 0;
	size_t m_height = 0;

	size_t m_mapDataSize = 0;

	size_t m_imgWidth = 0;
	size_t m_imgHeight = 0;

	std::map<int, std::map<int, Tile*>> m_mapData;//pointer (2d-array) to Map Infos
	MapTile m_border;

	std::string m_tileTexturePath;
	std::string m_tileTextureName;
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
	const std::string getCallbackType() const override;
	static Tile* castBodyCallback(ph::Body::Callback* c);

	bool isPassable() const;
};

#endif // MAP_H
