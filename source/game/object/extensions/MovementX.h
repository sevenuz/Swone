/*
 * MovementX.h
 *
 *  Created on: 24.11.2020
 *      Author: julius
 */

#ifndef SWONE_GAME_OBJECT_EXTENSIONS_MOVEMENTX_H
#define SWONE_GAME_OBJECT_EXTENSIONS_MOVEMENTX_H

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <cmath>

#include "util/reader/Reader.h"
#include "game/object/Extension.h"

#define ACCELERATION 20

class MovementX : public Extension {
public:
	MovementX(GameObject* obj, StringMapMap& setupMap);
	void applyConfig(StringMapMap& setupMap) override;

	void update(sf::Time t) override;
	void event(GameObject::Event e) override;
private:
	int m_direction = 1;
	float m_posVelX = 0;
	float m_accelerationX = 0;
};

#endif /* SWONE_GAME_OBJECT_EXTENSIONS_MOVEMENTX_H */
