/*
 * Player.h
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#ifndef SOURCE_GAME_PLAYER_H_
#define SOURCE_GAME_PLAYER_H_

#include <iostream>
#include <cmath>

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include <graphics/Animation.h>
#include <graphics/AnimatedSprite.h>

#include "Map.h"

#define GRAVITY 9.81
#define JUMP_FORCE 60
#define MOVE_FORCE 7
#define SPEED_FACTOR 0.5
#define WEIGHT 30

namespace PlayerAnimation {
	enum { LEFT, RIGHT, JUMP };
}

class Player {
public:

	Player(float x, float y);
	virtual ~Player();

	AnimatedSprite * getAnimatedSprite();
  bool isMoving();
  bool isFalling();
  bool isJumping();
  void toggleFalling(bool falling); // changes jumping and falling
  void stopFalling();
  void startFalling();
	void jump();
	void move(float fx);

	void update(sf::Time ellapsed);
	void event(sf::Event& e);
  void refreshJump(sf::Time ellapsed);
  void resetJump();

  sf::Vector2f& calculateVec(sf::Time ellapsed, sf::Vector2f newPos);
	sf::Vector2f& calculatePos(sf::Time ellapsed);
  void apply();
  void applyX();
  void applyY();

  sf::Vector2f getVec();
	sf::Vector2f getPos();

  sf::Vector2f getSpritePos();
  sf::FloatRect getSpriteBounds();

	sf::FloatRect getHitbox();
  sf::Vector2f& getHitboxLeftBotton();
  sf::Vector2f& getHitboxRightBotton();
  sf::Vector2f& getHitboxLeftBotton(sf::Vector2f pos);
  sf::Vector2f& getHitboxRightBotton(sf::Vector2f pos);
  sf::FloatRect& getHitboxBounds();

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
	sf::Vector2f m_pos = sf::Vector2f(0,0); // position in map
	sf::Vector2f m_vec = sf::Vector2f(0,0); // direction vector
  sf::Vector2f m_nextPos = sf::Vector2f(0,0); // position in map after next calculation
	sf::Vector2f m_nextVec = sf::Vector2f(0,0); // direction vector after next calculation
  // TODO
	sf::FloatRect m_hitbox = sf::FloatRect(0.1875,0.0,0.1094,0.5);

	sf::Texture m_texture;
	Animation m_ani_left;
	Animation m_ani_right;
	Animation m_ani_jump;
	Animation * m_ani;
	AnimatedSprite m_sprite;

  bool m_isJumping = false; // movement to top
  bool m_isFalling = false; // movement to bottom
  bool m_isMoving = false;  // movement on x
};

#endif /* SOURCE_GAME_PLAYER_H_ */
