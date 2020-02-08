/*
 * Player.h
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#ifndef SOURCE_GAME_PLAYER_H_
#define SOURCE_GAME_PLAYER_H_

#include <iostream>

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include <graphics/Animation.h>
#include <graphics/AnimatedSprite.h>

#include "Map.h"

#define JUMP_FORCE 3
#define MOVE_FORCE 1
#define SPEED_FACTOR 1
#define WEIGHT 3

namespace PlayerAnimation {
	enum { LEFT, RIGHT, JUMP };
}

class Player {
public:

	Player(float x, float y);
	virtual ~Player();

	sf::Vector2f getLeftBoundry();
	sf::Vector2f getRightBoundry();
	AnimatedSprite * getAnimatedSprite();
  bool isMoving();
	void jump();
	void move(float fx);

	void update(sf::Time ellapsed);
	void event(sf::Event& e);
  void refreshJump(sf::Time ellapsed);

  sf::Vector2f calculateVec(sf::Time ellapsed, sf::Vector2f newPos) const;
	sf::Vector2f calculatePos(sf::Time ellapsed) const;

  void setPos(sf::Vector2f pos);
  void setVec(sf::Vector2f pos);
private:
	
	void setAnimation(unsigned int ani);

	sf::Time m_jumpCooldownTime = sf::seconds(2); // required time to get jump
	sf::Time m_jumpCooldown = sf::seconds(0); // used time of jump cooldown
	unsigned int m_jumpsPossible = 2; // possible jumps without landing
	unsigned int m_jumps = 0; // count of current jumps
	float m_jf = JUMP_FORCE; // height of jump
	float m_mf = MOVE_FORCE; // width of movement
	float m_m = WEIGHT; // weight (for gravityforce)
	sf::Vector2f m_pos = sf::Vector2f(3,0); // position in map
	sf::Vector2f m_vec = sf::Vector2f(0,0); // direction vector

	sf::Vector2f m_left_boundry;
	sf::Vector2f m_right_boundry;

	sf::Texture m_texture;
	Animation m_ani_left;
	Animation m_ani_right;
	Animation m_ani_jump;
	Animation * m_ani;
	AnimatedSprite m_sprite;
};

#endif /* SOURCE_GAME_PLAYER_H_ */
