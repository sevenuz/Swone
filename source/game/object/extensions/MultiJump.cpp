/*
 * MultiJump.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "MultiJump.h"

MultiJump::MultiJump(GameObject* obj, std::map<std::string, StringMap>& setupMap) : Extension(obj)
{
	if(setupMap.count(Extension::CONTROLS_PARAGRAPH)){
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(CONTROLS_JUMP_NAME))
			m_key_up = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][CONTROLS_JUMP_NAME]);
	}
	resetJump();
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
			if(!m_correctOrient)
				jump();
		}
	}
}

void MultiJump::update(sf::Time ellapsed) {
	refreshJump(ellapsed);
	if(m_correctOrient) {
		m_obj->getBody()->SetOrientAngle(m_obj->getBody()->GetOrientAngle() * m_correction);
	}
}

void MultiJump::onTileCollision(ph::Manifold* manifold, Tile* t)
{
	const float angleTolerance = 20.0f;
	const float velocityTolerance = 0.0025f;
	if(std::abs(m_obj->getBody()->angularVelocity) < velocityTolerance &&
			std::abs(m_obj->getBody()->GetOrientAngle()) > angleTolerance) {
		m_correctOrient = true;
		m_obj->getBody()->isRotatingOnCollision = false;
	} else {
		m_correctOrient = false;
		m_obj->getBody()->isRotatingOnCollision = true;
	}
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
