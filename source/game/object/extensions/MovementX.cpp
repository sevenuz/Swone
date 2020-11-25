/*
 * MovementX.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "MovementX.h"
// necessary because of forward declaration in Extension.h
#include "game/object/GameObject.h"

MovementX::MovementX(GameObject* obj, std::map<std::string, StringMap>& setupMap) : Extension(obj)
{
	for(auto& p: setupMap){
		std::string paragraph = p.first;
		if(paragraph == GAMEOBJECT_CONTROLS_PARAGRAPH) {
			for(auto& s: p.second){
				const std::string k = s.first;
				const std::string v = s.second;
				if(k == GAMEOBJECT_CONTROLS_LEFT_NAME)
					m_key_left = (sf::Keyboard::Key)Helper::toInt(v);
				else if(k == GAMEOBJECT_CONTROLS_RIGHT_NAME)
					m_key_right = (sf::Keyboard::Key)Helper::toInt(v);
			}
		}
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
			m_obj->setVelX(0);
		}
		else if (event.key.code == m_key_right) {
			m_obj->setVelX(0);
		}
	}
}
