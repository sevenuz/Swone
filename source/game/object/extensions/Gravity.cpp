/*
 * Gravity.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */
#include "Gravity.h"

Gravity::Gravity(GameObject* obj) : Extension(obj)
{}

float Gravity::calculateDrag(const float drag, const float angle, const float speed) {
	return pow(speed, 2) * std::cos(angle * M_PI / 180.0) * drag * SCALE_DRAG_CONST;
}

void Gravity::calculateVel(sf::Time ellapsed, float gravity)
{
	// x linear, y accelerated
	// gravity is passed from map obj
	const float s = ellapsed.asSeconds();
	float fx = m_obj->getVel().x;

	float drag = calculateDrag(m_obj->getDrag(), m_obj->getObjTransform().getRotation(), m_obj->getVel().y);
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

void Gravity::onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom)
{
	if (leftBottom != MapTile::SPACE || rightBottom != MapTile::SPACE) {
		m_obj->stopFalling();
		m_obj->applyX();
	}
	else {
		m_obj->apply();
	}
}

void Gravity::onOutOfMap(MapTile border)
{
	onTiles(border, border, border, border);
}

