#ifndef SWONE_GAME_OBJECT_EXTENSION_H
#define SWONE_GAME_OBJECT_EXTENSION_H

#include "game/Map.h"
#include "game/object/GameObject.h"
#include "physics/Manifold.h"

// Interface, defining functionality which can be added to a GameObject
class Extension {
public:
	static constexpr const char* S_VELOCITY = "velocity";

	Extension(GameObject* obj) : m_obj(obj){};
	virtual ~Extension(){};
	virtual void applyConfig(StringMapMap& setupMap){};
	virtual void getConfig(StringMapMap& extensionMap){};
	virtual void event(GameObject::Event e){};
	virtual void update(sf::Time ellapsed){};
	virtual void onObjectCollision(ph::Manifold* manifold, GameObject* go){};
	virtual void onTileCollision(ph::Manifold* manifold, Tile* t){};

	virtual void updateLog(){};
protected:
	GameObject* m_obj;
	GameObject* getGameObjectByIdentifier(const std::string& identifier) { // TODO implementierung hier legit??
		return m_obj->m_idGoMapper(identifier);
	};
};

#endif

