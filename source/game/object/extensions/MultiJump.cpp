/*
 * MultiJump.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "MultiJump.h"

MultiJump::MultiJump(GameObject* obj, std::map<std::string, StringMap>& setupMap) : Extension(obj)
{
	for(auto& p: setupMap){
		std::string paragraph = p.first;
		if(paragraph == GAMEOBJECT_CONTROLS_PARAGRAPH) {
			for(auto& s: p.second){
				if(s.first == GAMEOBJECT_CONTROLS_JUMP_NAME)
					m_key_up = (sf::Keyboard::Key)Helper::toInt(s.second);
			}
		}
	}
}

void MultiJump::jump() {
	if (m_jumps > 0) {
		m_jumps--;
		// Startkraft nach oben
		m_obj->setVelY(-(m_obj->getPossibleVel().y));
	}
}

void MultiJump::event(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == m_key_up) {
			jump();
		}
	}
}

void MultiJump::update(sf::Time ellapsed) {
	refreshJump(ellapsed);
}

void MultiJump::refreshJump(sf::Time ellapsed) {
	m_jumpCooldown += ellapsed;
	if (m_jumpCooldown >= m_jumpCooldownTime) {
		if (m_jumps < m_jumpsPossible) {
			m_jumps++;
		}
		m_jumpCooldown = sf::seconds(0);
	}
}

void MultiJump::resetJump() {
	m_jumpCooldown = sf::seconds(0);
	m_jumps = m_jumpsPossible;
}
