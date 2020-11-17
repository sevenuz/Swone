/*
 * Player.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "Player.h"

Player::Player(std::map<std::string, StringMap>& setupMap) : GameObject(setupMap)
{
	for(auto& p: setupMap){
		std::string paragraph = p.first;
		if(paragraph == GAMEOBJECT_CONTROLS_PARAGRAPH) {
			for(auto& s: p.second){
				const std::string k = s.first;
				const std::string v = s.second;
				if(k==GAMEOBJECT_CONTROLS_LEFT_NAME)
					m_key_left = (sf::Keyboard::Key)Helper::toInt(v);
				if(k==GAMEOBJECT_CONTROLS_RIGHT_NAME)
					m_key_right = (sf::Keyboard::Key)Helper::toInt(v);
				if(k==GAMEOBJECT_CONTROLS_JUMP_NAME)
					m_key_up = (sf::Keyboard::Key)Helper::toInt(v);
				else
					Log::ger().log(k + " is not a GameObject option", Log::Label::Warning);
			}
		}
	}
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
		if (event.key.code == m_key_left) {
			setAnimation(AnimationType::Left);
			move(-m_possibleVel.x);
		}
		else if (event.key.code == m_key_right) {
			setAnimation(AnimationType::Right);
			move(m_possibleVel.x);
		}
		else if (event.key.code == m_key_up) {
			setAnimation(AnimationType::Up);
			jump();
		}
	}
	else if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == m_key_left) {
			move(0);
		}
		else if (event.key.code == m_key_right) {
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
