/*
 * MovementX.h
 *
 *  Created on: 24.11.2020
 *      Author: julius
 */

#ifndef SWONE_GAME_OBJECT_EXTENSIONS_MOVEMENTX_H
#define SWONE_GAME_OBJECT_EXTENSIONS_MOVEMENTX_H

#include <SFML/Graphics.hpp>

#include "util/reader/Reader.h"
#include "game/object/Extension.h"

class MovementX : public Extension {
public:
	MovementX(GameObject* obj, StringMapMap& setupMap);
	void applyConfig(StringMapMap& setupMap) override;

	void event(sf::Event& e) override;
private:
	sf::Keyboard::Key m_key_left = sf::Keyboard::Left;
	sf::Keyboard::Key m_key_right = sf::Keyboard::Right;

	float m_posVelX = 0;
};

#endif /* SWONE_GAME_OBJECT_EXTENSIONS_MOVEMENTX_H */
