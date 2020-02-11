#include "GameObject.h"

sf::Vector2f& GameObject::calculatePos(sf::Time ellapsed) {
    const float s = ellapsed.asSeconds();
    m_nextPos.x = m_pos.x + (m_vec.x * SPEED_FACTOR * s);
    m_nextPos.y = m_pos.y + (m_vec.y * SPEED_FACTOR * s);
    return m_nextPos;
}

sf::Vector2f& GameObject::calculateVec(sf::Time ellapsed, sf::Vector2f newPos) {
    return m_nextVec;
}

 void GameObject::onTiles(unsigned int leftTop, unsigned int rightTop, unsigned int leftBottom, unsigned int rightBottom) {
    m_pos = m_nextPos;
    m_vec = m_nextVec;
 }

sf::Vector2f& GameObject::getVec() {
    return m_vec;
}
sf::Vector2f& GameObject::getPos() {
    return m_pos;
}

sf::FloatRect GameObject::getHitbox(){
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

