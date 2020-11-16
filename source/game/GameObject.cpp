#include "GameObject.h"

GameObject::GameObject(std::string identifier, float x, float y) : m_identifier(identifier) {
	m_pos = sf::Vector2f(x, y);
}

GameObject::GameObject(std::map<std::string, StringMap>& setupMap)
{
	for(auto& p: setupMap){
		std::string paragraph = p.first;
		for(auto& s: p.second){
			std::string k = s.first;
			std::string v = s.second;
			if(k==GAMEOBJECT_NAME_NAME)
				Log::ger().log(v);
				//setName(v);
			else
				Log::ger().log(k + " is not a settings option", Log::Label::Warning);
		}
	}
}

sf::Vector2f& GameObject::calculatePos(sf::Time ellapsed) {
	const float s = ellapsed.asSeconds();
	m_nextPos.x = m_pos.x + (m_vel.x * SPEED_FACTOR * s);
	m_nextPos.y = m_pos.y + (m_vel.y * SPEED_FACTOR * s);
	return m_nextPos;
}

sf::Vector2f& GameObject::getVel() {
	return m_vel;
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

sf::Vector2f& GameObject::calculateVel(sf::Time ellapsed, float gravity) {
	// x linear, y accelerated
	// gravity is passed from map obj
	const float s = ellapsed.asSeconds();
	float fx = m_vel.x;

	float speed = m_vel.y / s;
	float drag = calculateDrag(m_drag, speed);
	float fy = m_vel.y + (gravity * s);
	fy = fy > 0 ? std::max(fy - (drag * s), 0.0f) : std::min(fy + (drag * s), 0.0f);

	// speed lock on tile width/height,
	// to prevent glitching on low frame rates
	// TODO is maximum useful or still posible to glitch?
	// or: implement line from oldPos to newPos with collision detection
	m_nextVel.x = fx < Map::TILE_WIDTH ? fx : Map::TILE_WIDTH;
	m_nextVel.y = fy < Map::TILE_HEIGHT ? fy : Map::TILE_HEIGHT;

	if (m_log)
		Log::ger().detailsUpdateValue(std::to_string(speed), "speed", m_identifier);

	return m_nextVel;
}

std::string GameObject::getIdentifier() const {
	return m_identifier;
}

void GameObject::setAnimation(AnimationType ani) {
	switch (ani) {
		case AnimationType::Up:
			m_ani = &m_ani_up;
			break;
		case AnimationType::Left:
			m_ani = &m_ani_left;
			break;
		case AnimationType::Right:
			m_ani = &m_ani_right;
			break;
		case AnimationType::Down:
			m_ani = &m_ani_down;
			break;
		case AnimationType::Steady:
			m_ani = &m_ani_steady;
			break;
		default:
			m_ani = &m_ani_steady;
			break;
		}
}

AnimatedSprite* GameObject::getAnimatedSprite() {
	return &m_sprite;
}

bool GameObject::isMoving() {
	return m_isMoving;
}

bool GameObject::isRising() {
	return m_isRising;
}

bool GameObject::isFalling() {
	return m_isFalling;
}

void GameObject::updateFlags() {
	m_isFalling = m_vel.y > 0;
	m_isRising = m_vel.y < 0;
	m_isMoving = m_vel.x != 0;
}

void GameObject::stopFalling() {
	m_vel.y = 0;
	m_nextVel.y = 0;
}

void GameObject::apply() {
	updateFlags();
	setPos(m_nextPos);
	setVel(m_nextVel);
}

void GameObject::applyX() {
	m_nextPos.y = m_pos.y;
	apply();
}

void GameObject::applyY() {
	m_nextPos.x = m_pos.x;
	apply();
}

void GameObject::update(sf::Time ellapsed) {
	if (isMoving()) {
		m_sprite.play(*m_ani);
		m_sprite.update(ellapsed);
	}
	else {
		m_sprite.stop();
	}
}

void GameObject::setPos(sf::Vector2f pos) {
	m_pos = pos;
	m_sprite.setPosition(Map::toMapPixelX(m_pos.x), Map::toMapPixelY(m_pos.y));
}

void GameObject::setVel(sf::Vector2f vel) {
	m_vel.x = vel.x;
	m_vel.y = vel.y;
}

sf::Vector2f GameObject::getSpritePos() {
	return m_sprite.getPosition();
}

void GameObject::onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom) {
	if (leftBottom != MapTile::SPACE || rightBottom != MapTile::SPACE) {
		stopFalling();
		applyX();
	}
	else {
		apply();
	}
}

void GameObject::onOutOfMap() {
	apply();
};

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(m_sprite, states);
}

void GameObject::toggleLogging()
{
	m_log = !m_log;
	if(m_log)
		Log::ger().detailsPutTexture(&m_texture, "gameObject_texture", m_identifier);
}
