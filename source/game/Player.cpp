/*
 * Player.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "Player.h"

Player::Player(float x, float y) {
	m_x = x;
	m_y = y;

	if (!m_texture.loadFromFile("../res/sprites/player.png")){
		std::cout << "Failed to load player spritesheet!" << std::endl;
	}

	    m_ani_jump.setSpriteSheet(m_texture);
	    m_ani_jump.addFrame(sf::IntRect(32, 0, 32, 32));
	    m_ani_jump.addFrame(sf::IntRect(64, 0, 32, 32));
	    m_ani_jump.addFrame(sf::IntRect(32, 0, 32, 32));
	    m_ani_jump.addFrame(sf::IntRect( 0, 0, 32, 32));

	    m_ani_left.setSpriteSheet(m_texture);
	    m_ani_left.addFrame(sf::IntRect(32, 32, 32, 32));
	    m_ani_left.addFrame(sf::IntRect(64, 32, 32, 32));
	    m_ani_left.addFrame(sf::IntRect(32, 32, 32, 32));
	    m_ani_left.addFrame(sf::IntRect( 0, 32, 32, 32));

	m_ani_right.setSpriteSheet(m_texture);
	m_ani_right.addFrame(sf::IntRect(32, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect(64, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect(32, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect( 0, 64, 32, 32));

	setAnimation(PlayerAnimation::LEFT);

	m_sprite.setPosition(sf::Vector2f(x, y));
	m_sprite.play(*m_ani);
}

Player::~Player() {
	// TODO Auto-generated destructor stub
}

void Player::setAnimation(unsigned int ani){
	switch(ani) {
	case PlayerAnimation::LEFT:
		m_ani = &m_ani_left;
		break;
	case PlayerAnimation::RIGHT:
		m_ani = &m_ani_right;
		break;
	case PlayerAnimation::JUMP:
		m_ani = &m_ani_jump;
		break;
	}
}

AnimatedSprite * Player::getAnimatedSprite(){
	return &m_sprite;
}

void Player::jump(){
	if(m_jumps > 0){
		m_jumps--;
		m_isJump = true;
		m_fy = -m_jf;
	}
}

void Player::move(float fx){
	m_fx = fx;
}

void Player::event(sf::Event& event) {
	if(event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Left) {
			m_animationFlag = true;
			setAnimation(PlayerAnimation::LEFT);
			move(-m_mf);
		} else if (event.key.code == sf::Keyboard::Right) {
			m_animationFlag = true;
			setAnimation(PlayerAnimation::RIGHT);
			move(m_mf);
		} else if (event.key.code == sf::Keyboard::Up) {
			m_animationFlag = true;
			setAnimation(PlayerAnimation::JUMP);
			jump();
		} else {
		}
	} else if(event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::Left) {
			move(0);
			m_animationFlag = false;
		} else if (event.key.code == sf::Keyboard::Right) {
			move(0);
			m_animationFlag = false;
		} else if (event.key.code == sf::Keyboard::Up) {
			m_animationFlag = false;
		} else {
		}
	}
}

void Player::update(sf::Time ellapsed){
	if(m_animationFlag || m_isJump){
		m_sprite.play(*m_ani);
		m_sprite.update(ellapsed);
	} else {
		m_sprite.stop();
	}
	calculateMove(ellapsed);
	calculateJump(ellapsed);
	m_sprite.setPosition(m_x, m_y);
}

void Player::calculateJump(sf::Time ellapsed){
	m_jumpCooldown += ellapsed;
	if(m_jumpCooldown >= m_jumpCooldownTime){
		if(m_jumps < m_jumpsPossible){
			m_jumps++;
		}
		m_jumpCooldown = sf::seconds(0);
	}
	if(m_y > 200){
		m_y = 200;
		m_fy = 0;
		m_isJump = false;
		return;
	}
	if(m_isJump){
		m_y = m_y + m_fy * ellapsed.asSeconds() * SPEED_FACTOR; 
		m_fy = m_fy + (ellapsed.asSeconds() * m_m);	
	}
}

void Player::calculateMove(sf::Time ellapsed){
	m_x = m_x + m_fx * ellapsed.asSeconds() * SPEED_FACTOR;
}
