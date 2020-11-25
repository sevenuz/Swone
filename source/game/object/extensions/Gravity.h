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

class Gravity : public Extension {
public:
	// controls
	static constexpr const char* GAMEOBJECT_CONTROLS_PARAGRAPH = "controls";
	static constexpr const char* GAMEOBJECT_CONTROLS_LEFT_NAME = "left";
	static constexpr const char* GAMEOBJECT_CONTROLS_RIGHT_NAME = "right";

	Gravity(GameObject* obj);
	static float calculateDrag(const float drag, const float angle, const float speed);

	void calculateVel(sf::Time ellapsed, float gravity) override;
	void calculatePos(sf::Time ellapsed) override;
	void onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom) override;
	void onOutOfMap(MapTile border) override;
private:
	sf::Keyboard::Key m_key_left = sf::Keyboard::Left;
	sf::Keyboard::Key m_key_right = sf::Keyboard::Right;
};

#endif
