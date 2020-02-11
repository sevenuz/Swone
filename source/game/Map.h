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

namespace MapTiles
{
enum { SHARP = 0, UNDERSCORE = 1, W = 2, SPACE = 3, DEFAULT = SPACE};
}

class Map: public sf::Drawable,public Handleable
{
public:
    static const unsigned int TILE_WIDTH = 64;
    static const unsigned int TILE_HEIGHT = 64;

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
    void scaleToFit(unsigned int w, unsigned int h);

    void setWidth(unsigned int);
    void setHeight(unsigned int);
    void setBorder(std::string);
    unsigned int getWidth();
    unsigned int getHeight();
    unsigned int getImageWidth();
    unsigned int getImageHeight();

    float getScale();

    std::string getBorder();
    unsigned int & getMapData();
    unsigned int getMapDataValue(unsigned int h, unsigned int w);
    void setMapDataValue(unsigned int h, unsigned int w, unsigned int v);
    void iniMapData();

    void createMapImage();
protected:
private:
    Controller & m_controller;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    std::string m_name= "";

    unsigned int m_width = 0;
    unsigned int m_height = 0;

    unsigned int m_mapDataSize = 0;

    unsigned int m_imgWidth = 0;
    unsigned int m_imgHeight = 0;

    float m_scale = 1;

    std::string m_border = "space";

    unsigned int * m_mapData;//pointer (2d-array) to Map Infos

    sf::Image m_mapTiles;//Img with all tiles
    sf::Image m_mapImage;//Img with map tiles
    sf::Texture m_texture;
    sf::Sprite m_sprite;

    bool m_mapDrawable = false;
};

#endif // MAP_H
