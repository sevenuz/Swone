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

#include "graphics/Animation.h"
#include "graphics/AnimatedSprite.h"
#include "Map.h"
#include "GameObject.h"

#define GRAVITY 25
#define JUMP_FORCE 15
#define MOVE_FORCE 7
#define WEIGHT 30

enum PlayerAnimation : char { LEFT, RIGHT, JUMP };

class Player: public GameObject {
public:
	Player(float x, float y);
	virtual ~Player();

  void update(sf::Time ellapsed) override;
  void event(sf::Event& e) override;

  sf::Vector2f& calculateVec(sf::Time ellapsed, sf::Vector2f newPos) override;
  sf::Vector2f& calculatePos(sf::Time ellapsed) override;
  void onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom) override;
  void onOutOfMap() override;

	AnimatedSprite * getAnimatedSprite();
  bool isMoving();
  bool isFalling();
  bool isJumping();
  void toggleFalling(bool falling); // changes jumping and falling
  void stopFalling();
  void startFalling();
	void jump();
	void move(float fx);

  void refreshJump(sf::Time ellapsed);
  void resetJump();

  void apply();
  void applyX();
  void applyY();

  sf::Vector2f getSpritePos();
  sf::FloatRect getSpriteBounds();

  void setPos(sf::Vector2f pos);
  void setVec(sf::Vector2f pos);
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void setAnimation(PlayerAnimation ani);

	sf::Time m_jumpCooldownTime = sf::seconds(2); // required time to get jump
	sf::Time m_jumpCooldown = sf::seconds(0); // used time of jump cooldown
	unsigned int m_jumpsPossible = 2; // possible jumps without landing
	unsigned int m_jumps = 0; // count of current jumps
	float m_jf = JUMP_FORCE; // height of jump
	float m_mf = MOVE_FORCE; // width of movement
	float m_m = WEIGHT; // weight (for gravityforce)

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
