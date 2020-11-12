#ifndef SWONE_GAME_GAMEOBJECT_H
#define SWONE_GAME_GAMEOBJECT_H

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include "Handleable.h"
#include "Map.h"

#define GRAVITY 25
#define SPEED_FACTOR 0.5
#define DEFAULT_DRAG 100

// This scales the drag, so that with a Gravity of 25 and a 
// drag property of 100, a terminal velocity of 50 is reached
#define SCALE_DRAG_CONST 0.0000000276f

class GameObject : public Handleable {
public:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	virtual void event(sf::Event& e) = 0;
	virtual void update(sf::Time ellapsed) = 0;

	// TODO standard implementation of this functions
	// calculate the next position of the object and save the result in m_nextPos
	virtual sf::Vector2f& calculateVec(sf::Time ellapsed, sf::Vector2f newPos);
	// calculate the next speed vct of the object and save the result in m_nextVec
	virtual sf::Vector2f& calculatePos(sf::Time ellapsed);
	// is called with the tiles which are hitting the corners
	// of the hitbox of the m_nextPos vector
	virtual void onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom);
	// if the object is complete out of map, this function is called
	virtual void onOutOfMap() {};

	virtual sf::Vector2f& getVec();
	virtual sf::Vector2f& getPos();

	virtual sf::FloatRect getHitbox();
	virtual sf::Vector2f getHitboxLeftTop(const sf::Vector2f& pos);
	virtual sf::Vector2f getHitboxRightTop(const sf::Vector2f& pos);
	virtual sf::Vector2f getHitboxLeftBottom(const sf::Vector2f& pos);
	virtual sf::Vector2f getHitboxRightBottom(const sf::Vector2f& pos);
	virtual sf::FloatRect getHitboxBounds();
protected:
	sf::Vector2f m_pos = sf::Vector2f(0, 0); // position in map
	sf::Vector2f m_vec = sf::Vector2f(0, 0); // direction vector
	sf::Vector2f m_nextPos = sf::Vector2f(0, 0); // position in map after next calculation
	sf::Vector2f m_nextVec = sf::Vector2f(0, 0); // direction vector after next calculation
	// TODO
	sf::FloatRect m_hitbox = sf::FloatRect(0.1875, 0.0, 0.1094, 0.5);

	// higher drag means slower falling
	float m_drag = DEFAULT_DRAG;
};

float calculateDrag(const float& drag, const float& speed);
#endif