#ifndef SWONE_CLIENT_GAME_OBJECT_EXTENSION_H
#define SWONE_CLIENT_GAME_OBJECT_EXTENSION_H

#include "client/game/object/GameObjectDrawing.h"

// Interface, defining functionality which can be added to a GameObject
class ExtensionDrawing {
public:
	ExtensionDrawing(){};
	virtual ~ExtensionDrawing(){};
	virtual void applyConfig(StringMapMap& setupMap){};
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{};
	virtual void update(sf::Time ellapsed){};
};

#endif

