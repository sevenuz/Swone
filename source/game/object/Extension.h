#ifndef SWONE_GAME_OBJECT_EXTENSION_H
#define SWONE_GAME_OBJECT_EXTENSION_H

#include "game/Map.h"
#include "game/object/GameObject.h"
#include "physics/Manifold.h"

// Interface, defining functionality which can be added to a GameObject
class Extension {
public:
	static constexpr const char* CONTROLS_PARAGRAPH = "controls";
	static constexpr const char* S_VELOCITY = "velocity";

	Extension(GameObject* obj) : m_obj(obj){};
	virtual ~Extension(){};
	virtual void applyConfig(std::map<std::string, StringMap>& setupMap){};
	virtual void getConfig(std::map<std::string, StringMap>& extensionMap){};
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{};
	virtual void event(sf::Event& e){};
	virtual void update(sf::Time ellapsed){};
	virtual void onObjectCollision(ph::Manifold* manifold, GameObject* go){};
	virtual void onTileCollision(ph::Manifold* manifold, Tile* t){};

	virtual void updateLog(){};
protected:
	GameObject* m_obj;
};

#endif

