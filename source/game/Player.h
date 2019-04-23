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
#define JUMP_FORCE 150
#define MOVE_FORCE 100
#define SPEED_FACTOR 1
#define WEIGHT 300

namespace PlayerAnimation {
	enum { LEFT, RIGHT, JUMP };
}

class Player {
public:

	Player(float x, float y);
	virtual ~Player();

	AnimatedSprite * getAnimatedSprite();
	void jump();
	void move(float fx);

	void update(sf::Time ellapsed);
	void event(sf::Event& e);
	void calculateJump(sf::Time ellapsed);
	void calculateMove(sf::Time ellapsed);
private:
	
	void setAnimation(unsigned int ani);

	sf::Time m_jumpCooldownTime = sf::seconds(2); // required time to get jump
	sf::Time m_jumpCooldown = sf::seconds(0); // used time of jump cooldown
	unsigned int m_jumpsPossible = 2; // possible jumps without landing
	unsigned int m_jumps = 0; // count of current jumps
	float m_jf = JUMP_FORCE; // height of jump
	float m_mf = MOVE_FORCE; // width of movement
	float m_m = WEIGHT; // weight (for gravityforce)
	float m_x = 0; // x position
	float m_y = 0; // y position
	float m_fx = 10; // vector x direction
	float m_fy = 0; // vector y direction

	bool m_isJump = false;
	bool m_animationFlag = false; // true if ani plays

	sf::Texture m_texture;
	Animation m_ani_left;
	Animation m_ani_right;
	Animation m_ani_jump;
	Animation * m_ani;
	AnimatedSprite m_sprite;

};

#endif /* SOURCE_GAME_PLAYER_H_ */
