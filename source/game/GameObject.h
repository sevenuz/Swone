#ifndef SWONE_GAME_GAMEOBJECT_H
#define SWONE_GAME_GAMEOBJECT_H

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include "Handleable.h"
#include "Map.h"

#define SPEED_FACTOR 0.5

class GameObject: public Handleable {
public:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
  virtual void event(sf::Event& e) = 0;
	virtual void update(sf::Time ellapsed) = 0;

  virtual sf::Vector2f& calculateVec(sf::Time ellapsed, sf::Vector2f newPos);
  virtual sf::Vector2f& calculatePos(sf::Time ellapsed);
  virtual void onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom);
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
  sf::Vector2f m_pos = sf::Vector2f(0,0); // position in map
	sf::Vector2f m_vec = sf::Vector2f(0,0); // direction vector
  sf::Vector2f m_nextPos = sf::Vector2f(0,0); // position in map after next calculation
	sf::Vector2f m_nextVec = sf::Vector2f(0,0); // direction vector after next calculation
  // TODO
	sf::FloatRect m_hitbox = sf::FloatRect(0.1875,0.0,0.1094,0.5);
};
#endif

