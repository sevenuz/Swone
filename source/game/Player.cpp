/*
 * Player.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "Player.h"

Player::Player(std::string identifier, float x, float y) : GameObject(identifier, x, y) {
	if (!m_texture.loadFromFile("../res/sprites/player.png")) {
		std::cout << "Failed to load player spritesheet!" << std::endl;
	}

	m_ani_up.setSpriteSheet(m_texture);
	m_ani_up.addFrame(sf::IntRect(32, 0, 32, 32));
	m_ani_up.addFrame(sf::IntRect(64, 0, 32, 32));
	m_ani_up.addFrame(sf::IntRect(32, 0, 32, 32));
	m_ani_up.addFrame(sf::IntRect(0, 0, 32, 32));

	m_ani_left.setSpriteSheet(m_texture);
	m_ani_left.addFrame(sf::IntRect(32, 32, 32, 32));
	m_ani_left.addFrame(sf::IntRect(64, 32, 32, 32));
	m_ani_left.addFrame(sf::IntRect(32, 32, 32, 32));
	m_ani_left.addFrame(sf::IntRect(0, 32, 32, 32));

	m_ani_right.setSpriteSheet(m_texture);
	m_ani_right.addFrame(sf::IntRect(32, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect(64, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect(32, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect(0, 64, 32, 32));

	setAnimation(AnimationType::Left);

	m_sprite.setPosition(sf::Vector2f(x, y));
	m_sprite.play(*m_ani);
}

Player::~Player() {
	// TODO Auto-generated destructor stub
}

void Player::jump() {
	if (m_jumps > 0) {
		m_jumps--;
		m_vel.y = -1 * m_possibleVel.y; // Startkraft nach oben
	}
}

void Player::move(float fx) {
	m_vel.x = fx;
}

void Player::event(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Left) {
			setAnimation(AnimationType::Left);
			move(-m_possibleVel.x);
		}
		else if (event.key.code == sf::Keyboard::Right) {
			setAnimation(AnimationType::Right);
			move(m_possibleVel.x);
		}
		else if (event.key.code == sf::Keyboard::Up) {
			setAnimation(AnimationType::Up);
			jump();
		}
	}
	else if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::Left) {
			move(0);
		}
		else if (event.key.code == sf::Keyboard::Right) {
			move(0);
		}
	}
}

void Player::update(sf::Time ellapsed) {
	GameObject::update(ellapsed);
	refreshJump(ellapsed);
}

void Player::refreshJump(sf::Time ellapsed) {
	m_jumpCooldown += ellapsed;
	if (m_jumpCooldown >= m_jumpCooldownTime) {
		if (m_jumps < m_jumpsPossible) {
			m_jumps++;
		}
		m_jumpCooldown = sf::seconds(0);
	}
}

void Player::resetJump() {
	m_jumpCooldown = sf::seconds(0);
	m_jumps = m_jumpsPossible;
}
