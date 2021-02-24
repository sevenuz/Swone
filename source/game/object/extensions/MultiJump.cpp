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
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH)) {
		// setup
		if(setupMap[Reader::DEFAULT_PARAGRAPH].count(S_JUMPS_POSSIBLE))
			m_jumpsPossible = Helper::toInt(setupMap[Reader::DEFAULT_PARAGRAPH][S_JUMPS_POSSIBLE]);
		if(setupMap[Reader::DEFAULT_PARAGRAPH].count(Extension::S_VELOCITY))
			m_posVelY = Helper::toVector2f(setupMap[Reader::DEFAULT_PARAGRAPH][Extension::S_VELOCITY]).y;
		// state
		if(setupMap[Reader::DEFAULT_PARAGRAPH].count(S_JUMPS))
			m_jumps = Helper::toInt(setupMap[Reader::DEFAULT_PARAGRAPH][S_JUMPS]);
	}
}

void MultiJump::getConfig(StringMapMap& extensionMap)
{
	extensionMap[Reader::DEFAULT_PARAGRAPH][S_JUMPS] = std::to_string(m_jumps);
}

void MultiJump::onTileCollision(ph::Manifold* manifold, Tile* t)
{
	resetJump();
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

void MultiJump::resetJump() {
	m_jumps = m_jumpsPossible;
}
