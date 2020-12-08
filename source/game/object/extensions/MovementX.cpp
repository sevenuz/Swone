/*
 * MovementX.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "MovementX.h"

MovementX::MovementX(GameObject* obj, std::map<std::string, StringMap>& setupMap) : Extension(obj)
{
	if(setupMap.count(Extension::CONTROLS_PARAGRAPH)){
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(CONTROLS_LEFT_NAME))
			m_key_left = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][CONTROLS_LEFT_NAME]);
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(CONTROLS_RIGHT_NAME))
			m_key_right = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][CONTROLS_RIGHT_NAME]);
	}
}

void MovementX::event(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == m_key_left) {
			m_obj->setVelX(-(m_obj->getPossibleVel().x));
		}
		else if (event.key.code == m_key_right) {
			m_obj->setVelX(m_obj->getPossibleVel().x);
		}
	}
	else if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == m_key_left) {
			//m_obj->setVelX(0);
		}
		else if (event.key.code == m_key_right) {
			//m_obj->setVelX(0);
		}
	}
}
