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

	if (!m_texture.loadFromFile("/home/julius/Dokumente/swone/newSwone/Swone/res/sprites/player.png")){
		std::cout << "Failed to load player spritesheet!" << std::endl;
	}

	m_ani_right.setSpriteSheet(m_texture);
	m_ani_right.addFrame(sf::IntRect(32, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect(64, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect(32, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect( 0, 64, 32, 32));

	m_ani = &m_ani_right;

	m_sprite.setPosition(sf::Vector2f(x, y));
	m_sprite.play(*m_ani);
}

Player::~Player() {
	// TODO Auto-generated destructor stub
}

AnimatedSprite * Player::getAnimatedSprite(){
	return &m_sprite;
}

void Player::jump(){
	m_fy = JUMP_FORCE;
}

void Player::event(sf::Event& event) {
	if(event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::Left) {
						m_sprite.move(-m_fx, m_fy);
					} else if (event.key.code == sf::Keyboard::Right) {
						m_sprite.move(m_fx, m_fy);
					}
	}
}

void Player::update(sf::Time ellapsed){
	m_sprite.update(ellapsed);
}
