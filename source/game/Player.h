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
#define JUMP_FORCE 20;

class Player {
public:

	Player(float x, float y);
	virtual ~Player();

	AnimatedSprite * getAnimatedSprite();
	void jump();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);

private:

	float m_x = 0; // x position
	float m_y = 0; // y position
	float m_fx = 10; // vector x direction
	float m_fy = 0; // vector y direction

	sf::Texture m_texture;
	Animation m_ani_left;
	Animation m_ani_right;
	Animation m_ani_jump;
	Animation * m_ani;
	AnimatedSprite m_sprite;

};

#endif /* SOURCE_GAME_PLAYER_H_ */
