/*
 * Gravity.h
 *
 *  Created on: 24.11.2020
 *      Author: julius
 */

#ifndef SWONE_GAME_OBJECT_EXTENSIONS_GRAVITY_H
#define SWONE_GAME_OBJECT_EXTENSIONS_GRAVITY_H

#include <SFML/Graphics.hpp>

#include "util/reader/Reader.h"
#include "game/object/Extension.h"

#define DEFAULT_DRAG 100

// This scales the drag, so that with a Gravity of 25 and a
// drag property of 100, a terminal velocity of 50 is reached
#define SCALE_DRAG_CONST 0.0000000276f


class Gravity : public Extension {
public:
	static constexpr const char* DRAG_NAME = "drag";

	Gravity(GameObject* obj, std::map<std::string, StringMap>& setupMap);
	static float calculateDrag(const float drag, const float angle, const float speed);

	void calculateVel(sf::Time ellapsed, float gravity) override;
	void calculatePos(sf::Time ellapsed) override;
	void onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom) override;
	void onOutOfMap(MapTile border) override;
private:
	// higher drag means slower falling
	float m_drag = DEFAULT_DRAG;
};

#endif
