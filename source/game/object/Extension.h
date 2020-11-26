#ifndef SWONE_GAME_OBJECT_EXTENSION_H
#define SWONE_GAME_OBJECT_EXTENSION_H

#include "game/Map.h"
#include "game/object/GameObject.h"

// Interface, defining functionality which can be added to a GameObject
class Extension {
public:
	static constexpr const char* CONTROLS_PARAGRAPH = "controls";

	Extension(GameObject* obj) : m_obj(obj){};
	virtual ~Extension(){};
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{};
	virtual void event(sf::Event& e){};
	virtual void update(sf::Time ellapsed){};
	virtual void calculateVel(sf::Time ellapsed, float gravity){};
	virtual void calculatePos(sf::Time ellapsed){};
	virtual void onTiles(Tile leftTop, Tile rightTop, Tile leftBottom, Tile rightBottom){};
	virtual void onOutOfMap(Tile border){};

	virtual void updateLog(){};
protected:
	GameObject* m_obj;
};

#endif

