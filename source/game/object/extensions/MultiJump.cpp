/*
 * MultiJump.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "MultiJump.h"

MultiJump::MultiJump(GameObject* obj, StringMapMap& setupMap) : Extension(obj)
{
	applyConfig(setupMap);
	resetJump();
}

void MultiJump::applyConfig(StringMapMap& setupMap)
{
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH))
		if(setupMap[Reader::DEFAULT_PARAGRAPH].count(Extension::S_VELOCITY))
			m_posVelY = Helper::toVector2f(setupMap[Reader::DEFAULT_PARAGRAPH][Extension::S_VELOCITY]).y;
}

void MultiJump::jump() {
	if (m_jumps > 0) {
		m_jumps--;
		// Startkraft nach oben
		m_obj->setVelY(-m_posVelY);
	}
}

void MultiJump::event(GameObject::Event event) {
	if (event.up) {
		jump();
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
