#include "GameObject.h"

GameObject::GameObject(std::string identifier, float x, float y, bool log) {
	this->identifier = identifier;
	m_pos = sf::Vector2f(x, y);
	detailsActive = log;
}

sf::Vector2f& GameObject::calculatePos(sf::Time ellapsed) {
	const float s = ellapsed.asSeconds();
	m_nextPos.x = m_pos.x + (m_vec.x * SPEED_FACTOR * s);
	m_nextPos.y = m_pos.y + (m_vec.y * SPEED_FACTOR * s);
	return m_nextPos;
}

void GameObject::onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom) {
	m_pos = m_nextPos;
	m_vec = m_nextVec;
}

sf::Vector2f& GameObject::getVec() {
	return m_vec;
}
sf::Vector2f& GameObject::getPos() {
	return m_pos;
}

sf::FloatRect GameObject::getHitbox() {
	return m_hitbox;
}

sf::Vector2f GameObject::getHitboxLeftTop(const sf::Vector2f& pos) {
	return sf::Vector2f((pos.x + m_hitbox.left), (pos.y + m_hitbox.top));
}
sf::Vector2f GameObject::getHitboxRightTop(const sf::Vector2f& pos) {
	return sf::Vector2f((pos.x + m_hitbox.left) + m_hitbox.width, (pos.y + m_hitbox.top));
}
sf::Vector2f GameObject::getHitboxLeftBottom(const sf::Vector2f& pos) {
	return sf::Vector2f((pos.x + m_hitbox.left), (pos.y + m_hitbox.top) + m_hitbox.height);
}
sf::Vector2f GameObject::getHitboxRightBottom(const sf::Vector2f& pos) {
	return sf::Vector2f((pos.x + m_hitbox.left) + m_hitbox.width, (pos.y + m_hitbox.top) + m_hitbox.height);
}
//Pixel in Map
sf::FloatRect GameObject::getHitboxBounds() {
	float x = (m_pos.x + m_hitbox.left) * Map::TILE_WIDTH;
	float y = (m_pos.y + m_hitbox.top) * Map::TILE_HEIGHT;
	float w = m_hitbox.width * Map::TILE_WIDTH;
	float h = m_hitbox.height * Map::TILE_HEIGHT;
	return sf::FloatRect(x, y, w, h);
}

// This function should probably be moved sooner or later
float calculateDrag(const float& drag, const float& speed) {
	return pow(speed, 2) * drag * SCALE_DRAG_CONST;
}

sf::Vector2f& GameObject::calculateVec(sf::Time ellapsed, sf::Vector2f newPos) {
	const float s = ellapsed.asSeconds();
	const float g = GRAVITY;
	float fx = m_vec.x;
	// x linear, y beschleunigt

	float speed = m_vec.y / s;
	float drag = calculateDrag(m_drag, speed);
	float fy = m_vec.y + (g * s);
	fy = fy > 0 ? std::max(fy - (drag * s), 0.0f) : std::min(fy + (drag * s), 0.0f);

	// speed lock on tile width/height,
	// to prevent glitching on low frame rates
	// TODO is maximum useful or still posible to glitch?
	// or: implement line from oldPos to newPos with collision detection
	m_nextVec.x = fx < Map::TILE_WIDTH ? fx : Map::TILE_WIDTH;
	m_nextVec.y = fy < Map::TILE_HEIGHT ? fy : Map::TILE_HEIGHT;

	if (detailsActive) 
		detailEntrees["speed"] = std::to_string(speed);

	return m_nextVec;
}

std::map<std::string, std::string> GameObject::getDetails() {
	return detailEntrees;
}

std::string GameObject::getIdentifier() {
	return identifier;
}