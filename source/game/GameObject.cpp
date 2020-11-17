#include "GameObject.h"

GameObject::GameObject(std::map<std::string, StringMap>& setupMap)
	: m_identifier(setupMap[Reader::DEFAULT_PARAGRAPH][GAMEOBJECT_ID_NAME])
{
	for(auto& s: setupMap[Reader::DEFAULT_PARAGRAPH]){
		const std::string k = s.first;
		const std::string v = s.second;
		if(k == GAMEOBJECT_NAME_NAME)
			setName(v);
		else if(k == GAMEOBJECT_VELOCITY_NAME)
			setPossibleVel(Helper::toVector2f(v));
		else if(k == GAMEOBJECT_COLOR_NAME)
			setColor(Helper::toColor(v));
		else if(k == GAMEOBJECT_TEXTURE_NAME)
			setTexturePath(v);
		else if(k == GAMEOBJECT_HITBOX_NAME)
			setHitbox(Helper::toFloatRect(v));
		else if(k == GAMEOBJECT_DRAG_NAME)
			setDrag(Helper::toFloat(v));
	}
	for(auto& p: setupMap){
		std::string paragraph = p.first;
		if(paragraph == GAMEOBJECT_ANI_UP_PARAGRAPH) {
			setAnimationFrames(m_ani_up, p.second);
		} else if(paragraph == GAMEOBJECT_ANI_LEFT_PARAGRAPH) {
			setAnimationFrames(m_ani_left, p.second);
		} else if(paragraph == GAMEOBJECT_ANI_RIGHT_PARAGRAPH) {
			setAnimationFrames(m_ani_right, p.second);
		} else if(paragraph == GAMEOBJECT_ANI_DOWN_PARAGRAPH) {
			setAnimationFrames(m_ani_down, p.second);
		} else if(paragraph == GAMEOBJECT_ANI_STEADY_PARAGRAPH) {
			setAnimationFrames(m_ani_steady, p.second);
		}
	}
}

void GameObject::setAnimationFrames(Animation& animation, StringMap& frames)
{
	if(m_texturePath.empty())
		throw std::invalid_argument("GameObject Texture is missing.");
	animation.setSpriteSheet(m_texture);
	for(auto& s: frames){
		animation.addFrame(Helper::toIntRect(s.second));
	}
}

sf::Vector2f& GameObject::calculatePos(sf::Time ellapsed) {
	const float s = ellapsed.asSeconds();
	m_nextPos.x = m_pos.x + (m_vel.x * SPEED_FACTOR * s);
	m_nextPos.y = m_pos.y + (m_vel.y * SPEED_FACTOR * s);
	return m_nextPos;
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

void GameObject::setAnimation()
{
	if(isMoving() && m_vel.x < 0)
		setAnimation(AnimationType::Left);
	else if(isMoving() && m_vel.x > 0)
		setAnimation(AnimationType::Right);
	else if(isFalling())
		setAnimation(AnimationType::Down);
	else if(isRising())
		setAnimation(AnimationType::Up);
	else
		setAnimation(AnimationType::Steady);
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
	m_sprite.play(*m_ani);
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
	updateFlags();
	setAnimation();
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
	m_sprite.update(ellapsed);
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

void GameObject::updateLog() const
{
	if(!m_log)
		return;
	Log& log = Log::ger();
	log.detailsPutValue(std::to_string(m_isFalling), "isFalling", m_identifier);
	log.detailsPutValue(std::to_string(m_isMoving), "isMoving", m_identifier);
	log.detailsPutValue(std::to_string(m_isRising), "isRising", m_identifier);
}

std::string GameObject::getIdentifier() const {
	return m_identifier;
}

std::string GameObject::getName()
{
	return m_name;
}

void GameObject::setName(std::string s)
{
	m_name = s;
}

sf::Color GameObject::getColor()
{
	return m_color;
}

void GameObject::setColor(sf::Color s)
{
	m_color = s;
}

std::string GameObject::getTexturePath()
{
	return m_texturePath;
}

void GameObject::setTexturePath(std::string s)
{
	m_texturePath = s;
	Log::ger().log("load: " + s);
	if (!m_texture.loadFromFile(m_texturePath)) {
		Log::ger().log(m_identifier + ": Failed to load texture", Log::Label::Error);
		throw std::invalid_argument("Failed to load texture");
	}
}

sf::Vector2f GameObject::getPossibleVel()
{
	return m_possibleVel;
}

void GameObject::setPossibleVel(sf::Vector2f s)
{
	m_possibleVel = s;
}

sf::FloatRect GameObject::getHitbox() {
	return m_hitbox;
}

void GameObject::setHitbox(sf::FloatRect f)
{
	m_hitbox = f;
}

float GameObject::getDrag()
{
	return m_drag;
}

void GameObject::setDrag(float s)
{
	m_drag = s;
}

sf::Vector2f& GameObject::getVel() {
	return m_vel;
}
sf::Vector2f& GameObject::getPos() {
	return m_pos;
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

void GameObject::setPos(sf::Vector2f pos) {
	m_pos = pos;
	m_sprite.setPosition(Map::toMapPixelX(m_pos.x), Map::toMapPixelY(m_pos.y));
}

void GameObject::setVel(sf::Vector2f vel) {
	m_vel = vel;
}

sf::Vector2f GameObject::getSpritePos() {
	return m_sprite.getPosition();
}
