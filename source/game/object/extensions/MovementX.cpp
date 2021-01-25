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

	if(setupMap.count(Extension::CONTROLS_PARAGRAPH)){
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(Extension::CONTROLS_LEFT))
			m_key_left = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][Extension::CONTROLS_LEFT]);
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(Extension::CONTROLS_RIGHT))
			m_key_right = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][Extension::CONTROLS_RIGHT]);
	}
}

void MovementX::event(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == m_key_left) {
			m_obj->setVelX(-m_posVelX);
		}
		else if (event.key.code == m_key_right) {
			m_obj->setVelX(m_posVelX);
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
