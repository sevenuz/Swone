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
	// controls
	static constexpr const char* GAMEOBJECT_CONTROLS_PARAGRAPH = "controls";
	static constexpr const char* GAMEOBJECT_CONTROLS_LEFT_NAME = "left";
	static constexpr const char* GAMEOBJECT_CONTROLS_RIGHT_NAME = "right";

	MovementX(GameObject* obj, std::map<std::string, StringMap>& setupMap);
	void event(sf::Event& e) override;
private:
	sf::Keyboard::Key m_key_left = sf::Keyboard::Left;
	sf::Keyboard::Key m_key_right = sf::Keyboard::Right;
};

#endif /* SWONE_GAME_OBJECT_EXTENSIONS_MOVEMENTX_H */
