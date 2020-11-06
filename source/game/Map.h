#ifndef MAP_H
#define MAP_H

#include <string>
#include <stdexcept>
#include <Controller.h>
#include <Handleable.h>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <SFML/Graphics/Rect.hpp>

enum MapTile : char{ SHARP = 0, UNDERSCORE = 1, W = 2, SPACE = 3, DEFAULT = SPACE};

class Map: public sf::Drawable,public Handleable
{
public:
    static const size_t TILE_WIDTH = 64;
    static const size_t TILE_HEIGHT = 64;

    static float toMapPixelX(float x);
    static float toMapPixelY(float y);

    Map(Controller & c);
    virtual ~Map();

    void update(sf::Time ellapsed);
    void event(sf::Event& e);

    void setName(std::string);
    std::string getName();

    sf::Sprite & getSprite();

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
    MapTile & getMapData();
    MapTile getMapDataValue(size_t h, size_t w);
    void setMapDataValue(size_t h, size_t w, MapTile v);
    void iniMapData();

    void createMapImage();
protected:
private:
    Controller & m_controller;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    std::string m_name= "";

    size_t m_width = 0;
    size_t m_height = 0;

    size_t m_mapDataSize = 0;

    size_t m_imgWidth = 0;
    size_t m_imgHeight = 0;

    float m_scale = 1;

    MapTile * m_mapData;//pointer (2d-array) to Map Infos
    MapTile m_border;

    sf::Image m_mapTiles;//Img with all tiles
    sf::Image m_mapImage;//Img with map tiles
    sf::Texture m_texture;
    sf::Sprite m_sprite;

    bool m_mapDrawable = false;
};

#endif // MAP_H
