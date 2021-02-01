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

void MovementX::event(GameObject::Event event) {
	if (event.left) {
		m_obj->setVelX(-m_posVelX);
	} else {
		//m_obj->setVelX(0);
	}
	if (event.right) {
		m_obj->setVelX(m_posVelX);
	} else {
		//m_obj->setVelX(0);
	}
}
