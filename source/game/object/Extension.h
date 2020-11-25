#ifndef SWONE_GAME_OBJECT_EXTENSION_H
#define SWONE_GAME_OBJECT_EXTENSION_H

#include "game/Map.h"
// forward declaration to avoid dependency cyclus
class GameObject;

// Interface, defining functionality which can be added to a GameObject
class Extension {
public:
	Extension(GameObject* obj) : m_obj(obj){};
	virtual ~Extension(){};
	virtual void event(sf::Event& e){};
	virtual void update(sf::Time ellapsed){};
	virtual void calculateVel(sf::Time ellapsed, float gravity){};
	virtual void calculatePos(sf::Time ellapsed){};
	virtual void onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom){};
	virtual void onOutOfMap(MapTile border){};

	virtual void updateLog(){};
protected:
	GameObject* m_obj;
};

#endif

