#include "GameObject.h"

// necessary because of forward declaration in GameObject.h
#include "game/object/extensions/Gravity.h"
#include "game/object/extensions/MovementX.h"
#include "game/object/extensions/MultiJump.h"

GameObject::GameObject(std::map<std::string, StringMap>& setupMap)
	: m_identifier(setupMap[Reader::DEFAULT_PARAGRAPH][GAMEOBJECT_ID_NAME])
{
	for(auto& s: setupMap[Reader::DEFAULT_PARAGRAPH]){
		const std::string k = s.first;
		const std::string v = s.second;
		if(k == GAMEOBJECT_NAME_NAME)
			setName(v);
		else if(k == GAMEOBJECT_INITIAL_POS_NAME)
			setPos(Helper::toVector2f(v));
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

	if(setupMap[GAMEOBJECT_EXTENSIONS_PARAGRAPH].size()>0){
		if(Helper::toBool(setupMap[GAMEOBJECT_EXTENSIONS_PARAGRAPH][GAMEOBJECT_GRAVITY_EXTENSION]))
			m_extensions.push_back(new Gravity(this));
		if(Helper::toBool(setupMap[GAMEOBJECT_EXTENSIONS_PARAGRAPH][GAMEOBJECT_MOVEMENTX_EXTENSION]))
			m_extensions.push_back(new MovementX(this, setupMap));
		if(Helper::toBool(setupMap[GAMEOBJECT_EXTENSIONS_PARAGRAPH][GAMEOBJECT_MULTIJUMP_EXTENSION]))
			m_extensions.push_back(new MultiJump(this, setupMap));
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

void GameObject::calculatePos(sf::Time ellapsed) {
	for(Extension* e : m_extensions) e->calculatePos(ellapsed);
}

sf::Vector2f GameObject::getHitboxLeftTop(const sf::Vector2f& pos) {
	return m_objTransform.getTransform().transformPoint((m_hitbox.left), (m_hitbox.top)) + sf::Vector2f(pos.x, pos.y);
}
sf::Vector2f GameObject::getHitboxRightTop(const sf::Vector2f& pos) {
	return m_objTransform.getTransform().transformPoint((m_hitbox.left) + m_hitbox.width, (m_hitbox.top)) + sf::Vector2f(pos.x, pos.y);
}
sf::Vector2f GameObject::getHitboxLeftBottom(const sf::Vector2f& pos) {
	return m_objTransform.getTransform().transformPoint((m_hitbox.left), (m_hitbox.top) + m_hitbox.height) + sf::Vector2f(pos.x, pos.y);
}
sf::Vector2f GameObject::getHitboxRightBottom(const sf::Vector2f& pos) {
	return m_objTransform.getTransform().transformPoint((m_hitbox.left) + m_hitbox.width, (m_hitbox.top) + m_hitbox.height) + sf::Vector2f(pos.x, pos.y);
}
//Pixel in MapPixel relative to Obj
sf::FloatRect GameObject::getHitboxBounds() const {
	float x = m_hitbox.left * Map::TILE_WIDTH;
	float y = m_hitbox.top * Map::TILE_HEIGHT;
	float w = m_hitbox.width * Map::TILE_WIDTH;
	float h = m_hitbox.height * Map::TILE_HEIGHT;
	return sf::FloatRect(x, y, w, h);
}

void GameObject::calculateVel(sf::Time ellapsed, float gravity) {
	for(Extension* e : m_extensions) e->calculateVel(ellapsed, gravity);
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
	for(Extension* e : m_extensions) e->update(ellapsed);
}

void GameObject::event(sf::Event& ev) {
	for(Extension* e : m_extensions) e->event(ev);
}

void GameObject::onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom) {
	for(Extension* e : m_extensions) e->onTiles(leftTop, rightTop, leftBottom, rightBottom);
}

void GameObject::onOutOfMap(MapTile border) {
	for(Extension* e : m_extensions) e->onOutOfMap(border);
};

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform() * m_objTransform.getTransform();
	target.draw(m_sprite, states);

	if(!m_showHitbox)
		return;

	sf::FloatRect h = getHitboxBounds();
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(h.width, h.height));
	rectangle.setOutlineColor(sf::Color::Red);
	rectangle.setOutlineThickness(1);
	rectangle.setFillColor(sf::Color::Transparent);
	rectangle.setPosition(h.left, h.top);
	target.draw(rectangle, states);

}

void GameObject::toggleLogging()
{
	m_log = !m_log;
	if(m_log) {
		Log::ger().detailsPutValue(&m_texture, "gameObject_texture", m_identifier);
		Log::ger().detailsPutValue(&m_sprite, "animation", m_identifier);
	}
}

void GameObject::updateLog() const
{
	if(!m_log)
		return;
	Log& log = Log::ger();
	log.detailsPutValue(std::to_string(m_isFalling), "isFalling", m_identifier);
	log.detailsPutValue(std::to_string(m_isMoving), "isMoving", m_identifier);
	log.detailsPutValue(std::to_string(m_isRising), "isRising", m_identifier);
	log.detailsUpdateValue(std::to_string(m_pos.x), "current_x", m_identifier);
	log.detailsUpdateValue(std::to_string(m_pos.y), "current_y", m_identifier);
	log.detailsUpdateValue(std::to_string(m_vel.x), "vel_x", m_identifier);
	log.detailsUpdateValue(std::to_string(m_vel.y), "vel_y", m_identifier);

	for(Extension* e : m_extensions) e->updateLog();
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
	m_sprite.setColor(s);
}

std::string GameObject::getTexturePath()
{
	return m_texturePath;
}

void GameObject::setTexturePath(std::string s)
{
	m_texturePath = s;
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

sf::Transformable GameObject::getObjTransform() const
{
	return m_objTransform;
}


void GameObject::setAngle(float s)
{
	m_objTransform.setRotation(s);
}

void GameObject::setScale(float s)
{
	m_objTransform.setScale(s, s);
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
	setPosition(Map::toMapPixelX(m_pos.x), Map::toMapPixelY(m_pos.y));
}

void GameObject::setVel(sf::Vector2f vel) {
	m_vel = vel;
}

void GameObject::setVelX(float pos)
{
	m_vel.x = pos;
}

void GameObject::setVelY(float pos)
{
	m_vel.y = pos;
}

sf::Vector2f& GameObject::getNextPos()
{
	return m_nextPos;
}

void GameObject::setNextPos(sf::Vector2f pos)
{
	m_nextPos = pos;
}

sf::Vector2f& GameObject::getNextVel()
{
	return m_nextVel;
}

void GameObject::setNextVel(sf::Vector2f pos)
{
	m_nextVel = pos;
}


sf::Vector2f GameObject::getSpritePos() {
	return m_sprite.getPosition();
}
