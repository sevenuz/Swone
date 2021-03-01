/*
 * OrientCorrection.cpp
 *
 *  Created on: 14.12.2020
 *      Author: julius
 */

#include "OrientCorrection.h"

OrientCorrection::OrientCorrection(GameObject* obj) : Extension(obj)
{}

bool OrientCorrection::checkOrient() {
	const float angleTolerance = 5.0f;
	const float velocityTolerance = 0.25f;
	if(std::abs(m_obj->getBody()->angularVelocity) < velocityTolerance &&
			std::abs(m_obj->getBody()->GetOrientAngle()) > angleTolerance) {
		m_correctOrient = true;
	} else {
		m_correctOrient = false;
	}
	return m_correctOrient;
}

void OrientCorrection::update(sf::Time ellapsed) {
	// steadyness while walking
	if(!m_obj->isRising() && !m_obj->isFalling() && m_obj->isMoving())
		m_obj->getBody()->rotatable = false;
	else
		m_obj->getBody()->rotatable = true;

	if(m_correctOrient) {
		m_obj->getBody()->SetOrientAngle(m_obj->getBody()->GetOrientAngle() * m_correction);
		checkOrient();
	}
}

void OrientCorrection::onTileCollision(ph::Manifold* manifold, Tile* t)
{
	checkOrient();
}

void OrientCorrection::onObjectCollision(ph::Manifold* manifold, GameObject* t)
{
	checkOrient();
}
