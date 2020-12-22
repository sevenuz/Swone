/*
 * MultiJump.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "MultiJump.h"

MultiJump::MultiJump(GameObject* obj, std::map<std::string, StringMap>& setupMap) : Extension(obj)
{
	applyConfig(setupMap);
	resetJump();
}

void MultiJump::applyConfig(std::map<std::string, StringMap>& setupMap)
{
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH))
		if(setupMap[Reader::DEFAULT_PARAGRAPH].count(Extension::S_VELOCITY))
			m_posVelY = Helper::toVector2f(setupMap[Reader::DEFAULT_PARAGRAPH][Extension::S_VELOCITY]).y;

	if(setupMap.count(Extension::CONTROLS_PARAGRAPH)){
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(CONTROLS_JUMP_NAME))
			m_key_up = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][CONTROLS_JUMP_NAME]);
	}
}

void MultiJump::jump() {
	if (m_jumps > 0) {
		m_jumps--;
		// Startkraft nach oben
		m_obj->setVelY(-m_posVelY);
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
