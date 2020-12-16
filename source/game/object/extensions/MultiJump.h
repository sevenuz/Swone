/*
 * MultiJump.h
 *
 *  Created on: 24.11.2020
 *      Author: julius
 */

#ifndef SWONE_GAME_OBJECT_EXTENSIONS_MULTIJUMP_H
#define SWONE_GAME_OBJECT_EXTENSIONS_MULTIJUMP_H

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include "util/reader/Reader.h"
#include "game/object/Extension.h"

class MultiJump : public Extension {
public:
	// in controls paragraph
	static constexpr const char* CONTROLS_JUMP_NAME = "jump";

	MultiJump(GameObject* obj, std::map<std::string, StringMap>& setupMap);
	void event(sf::Event& e) override;
	void update(sf::Time ellapsed) override;

	void jump();
	void refreshJump(sf::Time ellapsed);
	void resetJump();
private:
	sf::Time m_jumpCooldownTime = sf::seconds(2); // required time to get jump
	sf::Time m_jumpCooldown = sf::seconds(0); // used time of jump cooldown

	unsigned int m_jumpsPossible = 2; // possible jumps without landing
	unsigned int m_jumps = 0; // count of current jumps

	sf::Keyboard::Key m_key_up = sf::Keyboard::Up;

	float m_posVelY = 0;
};

#endif /* SWONE_GAME_OBJECT_EXTENSIONS_MULTIJUMP_H */
