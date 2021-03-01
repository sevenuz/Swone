/*
 * MovementX.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "MovementX.h"

MovementX::MovementX(GameObject* obj, StringMapMap& setupMap) : Extension(obj)
{
	applyConfig(setupMap);
}

void MovementX::applyConfig(StringMapMap& setupMap)
{
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH))
		if(setupMap[Reader::DEFAULT_PARAGRAPH].count(Extension::S_VELOCITY))
			m_posVelX = Helper::toVector2f(setupMap[Reader::DEFAULT_PARAGRAPH][Extension::S_VELOCITY]).x;
}

void MovementX::update(sf::Time ellapsed)
{
	if(m_accelerationX == 0)
		return;
	float v = std::abs(m_obj->getVel().x) + ellapsed.asSeconds() * m_accelerationX;
	if(v <= m_posVelX) {
		m_obj->setVelX(v * m_direction);
	} else {
		m_obj->setVelX(m_posVelX * m_direction);
	}
}

void MovementX::event(GameObject::Event event) {
	m_accelerationX = ACCELERATION;
	if (event.left) {
		m_direction = -1;
	} else if (event.right) {
		m_direction = 1;
	} else {
		m_accelerationX = 0;
	}
}
