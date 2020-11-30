/*
 * Gravity.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */
#include "Gravity.h"

Gravity::Gravity(GameObject* obj, std::map<std::string, StringMap>& setupMap) : Extension(obj)
{
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH)){
		if(setupMap[Reader::DEFAULT_PARAGRAPH].count(DRAG_NAME))
			m_drag = Helper::toFloat(setupMap[Reader::DEFAULT_PARAGRAPH][DRAG_NAME]);
	}
}

void Gravity::stopFalling(float y) {
	m_obj->setNextVelY(0);
	m_obj->setNextPosY(y-m_obj->getHitbox().height);
}

void Gravity::stopRising(float y) {
	m_obj->setNextVelY(0);
	m_obj->setNextPosY(y + 1);
}

void Gravity::stopRight(float x) {
	m_obj->setNextVelX(0);
	m_obj->setNextPosX(x - m_obj->getHitbox().width - m_obj->getHitbox().left - 0.0001);
}

void Gravity::stopLeft(float x) {
	m_obj->setNextVelX(0);
	m_obj->setNextPosX(x + 1.0001 - m_obj->getHitbox().left);
}

float Gravity::calculateDrag(const float drag, const float angle, const float speed) {
	return pow(speed, 2) * std::cos(angle * M_PI / 180.0) * drag * SCALE_DRAG_CONST;
}

void Gravity::calculateVel(sf::Time ellapsed, float gravity)
{
	// x linear, y accelerated
	// gravity is passed from map obj
	const float s = ellapsed.asSeconds();
	float fx = m_obj->getVel().x;

	float drag = calculateDrag(m_drag, m_obj->getObjTransform().getRotation(), m_obj->getVel().y);
	float fy = m_obj->getVel().y + (gravity * s);
	fy = fy > 0 ? std::max(fy - (drag * s), 0.0f) : std::min(fy + (drag * s), 0.0f);

	// speed lock on tile width/height,
	// to prevent glitching on low frame rates
	// TODO is maximum useful or still posible to glitch?
	// or: implement line from oldPos to newPos with collision detection
	fx = fx < Map::TILE_WIDTH ? fx : Map::TILE_WIDTH;
	fy = fy < Map::TILE_HEIGHT ? fy : Map::TILE_HEIGHT;
	m_obj->setNextVel(sf::Vector2f(fx,fy));
}

void Gravity::calculatePos(sf::Time ellapsed)
{
	const float s = ellapsed.asSeconds();
	const float x = m_obj->getPos().x + (m_obj->getVel().x * SPEED_FACTOR * s);
	const float y = m_obj->getPos().y + (m_obj->getVel().y * SPEED_FACTOR * s);
	m_obj->setNextPos(sf::Vector2f(x,y));
}

void Gravity::onTilesY(Tile left, Tile right)
{
	if (left.isPassable() != right.isPassable() && m_obj->getVel().x!=0.0)
		return;
	if (m_obj->getVel().y > 0) {
		if (!left.isPassable() || !right.isPassable()) {
			stopFalling(right.pos.y);
		}
	}
	else if (m_obj->getVel().y < 0) {
		if (!left.isPassable() || !right.isPassable()) {
			stopRising(left.pos.y);
		}
	}
	else if (!left.isPassable() || !right.isPassable()) {
		m_obj->setNextVelY(0.0);
	}
}

void Gravity::onTilesX(Tile top, Tile bottom)
{
	if (m_obj->getVel().x > 0) {
		if (!top.isPassable() || !bottom.isPassable()) {
			stopRight(top.pos.x);
		}
	}
	else if (m_obj->getVel().x < 0) {
		if (!top.isPassable() || !bottom.isPassable()) {
			stopLeft(top.pos.x);
		}
	}
}

void Gravity::onOutOfMap(Tile border)
{
}

