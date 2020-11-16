/*
 * Player.h
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#ifndef SWONE_GAME_PLAYER_H_
#define SWONE_GAME_PLAYER_H_

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include "Map.h"
#include "GameObject.h"

class Player : public GameObject {
public:
	Player(std::string identifier, float x, float y);
	virtual ~Player();

	void update(sf::Time ellapsed) override;
	void event(sf::Event& e) override;

	void jump();
	void move(float fx);

	void refreshJump(sf::Time ellapsed);
	void resetJump();
private:
	sf::Time m_jumpCooldownTime = sf::seconds(2); // required time to get jump
	sf::Time m_jumpCooldown = sf::seconds(0); // used time of jump cooldown

	unsigned int m_jumpsPossible = 2; // possible jumps without landing
	unsigned int m_jumps = 0; // count of current jumps
};

#endif /* SOURCE_GAME_PLAYER_H_ */
