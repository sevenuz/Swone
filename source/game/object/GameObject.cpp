#include "GameObject.h"

// necessary because of forward declaration in GameObject.h
#include "game/object/extensions/Gravity.h"
#include "game/object/extensions/MovementX.h"
#include "game/object/extensions/MultiJump.h"
#include "game/object/extensions/Inventory.h"

GameObject::GameObject(std::map<std::string, StringMap>& setupMap)
	: m_identifier(setupMap[Reader::DEFAULT_PARAGRAPH][GAMEOBJECT_ID_NAME])
{
	// TODO remove loop?
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
	if(setupMap.count(GAMEOBJECT_EXTENSIONS_PARAGRAPH)){
		if(Helper::toBool(setupMap[GAMEOBJECT_EXTENSIONS_PARAGRAPH][GAMEOBJECT_GRAVITY_EXTENSION]))
			m_extensions.push_back(new Gravity(this, setupMap));
		if(Helper::toBool(setupMap[GAMEOBJECT_EXTENSIONS_PARAGRAPH][GAMEOBJECT_MOVEMENTX_EXTENSION]))
			m_extensions.push_back(new MovementX(this, setupMap));
		if(Helper::toBool(setupMap[GAMEOBJECT_EXTENSIONS_PARAGRAPH][GAMEOBJECT_MULTIJUMP_EXTENSION]))
			m_extensions.push_back(new MultiJump(this, setupMap));
		if(Helper::toBool(setupMap[GAMEOBJECT_EXTENSIONS_PARAGRAPH][GAMEOBJECT_INVENTORY_EXTENSION]))
			m_extensions.push_back(new Inventory(this, setupMap));
	}
}

void GameObject::setAnimationFrames(Animation& animation, StringMap& frames)
{
	// TODO read frameTime, read Frames in order from 1,...,n
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

void GameObject::setMovementAnimationAutomatic()
{
	if(!m_movementAnimationAutomatic)
		return;
	if(isMoving() && m_vel.x < 0)
		setMovementAnimation(MovementAnimation::Left);
	else if(isMoving() && m_vel.x > 0)
		setMovementAnimation(MovementAnimation::Right);
	else if(isFalling())
		setMovementAnimation(MovementAnimation::Down);
	else if(isRising())
		setMovementAnimation(MovementAnimation::Up);
	else
		setMovementAnimation(MovementAnimation::Steady);
}

void GameObject::setMovementAnimation(MovementAnimation ani) {
	switch (ani) {
		case MovementAnimation::Up:
			setAnimation(m_ani_up);
			break;
		case MovementAnimation::Left:
			setAnimation(m_ani_left);
			break;
		case MovementAnimation::Right:
			setAnimation(m_ani_right);
			break;
		case MovementAnimation::Down:
			setAnimation(m_ani_down);
			break;
		case MovementAnimation::Steady:
			setAnimation(m_ani_steady);
			break;
		default:
			setAnimation(m_ani_steady);
			break;
	}
}

void GameObject::playAnimationOnce(Animation& animation, std::function<void()> endCb)
{
	setMovementAnimationAutomatic(false, false);
	if(endCb == NULL)
		endCb = [&](){
			setMovementAnimationAutomatic(true, true);
		};
	m_sprite.setEndCallback(endCb);
	setAnimation(animation);
}

void GameObject::setAnimation(Animation& animation)
{
	// sets animation automaticly if pointer changed
	m_sprite.play(animation);
}

void GameObject::updateFlags() {
	m_isFalling = m_vel.y > 0;
	m_isRising = m_vel.y < 0;
	m_isMoving = m_vel.x != 0;
}

void GameObject::apply() {
	setPos(m_nextPos);
	setVel(m_nextVel);
	updateFlags();
	setMovementAnimationAutomatic();
}

void GameObject::update(sf::Time ellapsed) {
	m_sprite.update(ellapsed);
	for(Extension* e : m_extensions) e->update(ellapsed);
}

void GameObject::event(sf::Event& ev) {
	for(Extension* e : m_extensions) e->event(ev);
}

void GameObject::onTilesY(Tile top, Tile bottom) {
	for(Extension* e : m_extensions) e->onTilesY(top, bottom);
}

void GameObject::onTilesX(Tile left, Tile right) {
	for (Extension* e : m_extensions) e->onTilesX(left, right);
}

void GameObject::onOutOfMap(Tile border) {
	for(Extension* e : m_extensions) e->onOutOfMap(border);
};

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if(m_visible) {
		states.transform *= getTransform() * m_objTransform.getTransform();
		target.draw(m_sprite, states);
	}

	if(m_log) {
		sf::FloatRect h = getHitboxBounds();
		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(h.width, h.height));
		rectangle.setOutlineColor(sf::Color::Red);
		rectangle.setOutlineThickness(1);
		rectangle.setFillColor(sf::Color::Transparent);
		rectangle.setPosition(h.left, h.top);
		target.draw(rectangle, states);
	}

	for(Extension* e : m_extensions) e->draw(target, states);
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

AnimatedSprite* GameObject::getAnimatedSprite() {
	return &m_sprite;
}

bool GameObject::isVisible()
{
	return m_visible;
}

void GameObject::setVisible(bool s)
{
	m_visible = s;
}

bool GameObject::isMovementAnimationAutomatic()
{
	return m_movementAnimationAutomatic;
}

void GameObject::setMovementAnimationAutomatic(bool s, bool looped)
{
	m_movementAnimationAutomatic = s;
	m_sprite.setLooped(looped);
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

sf::Vector2f& GameObject::getPos() {
	return m_pos;
}

void GameObject::setPos(sf::Vector2f pos) {
	m_pos = pos;
	setPosition(Map::toMapPixelX(m_pos.x), Map::toMapPixelY(m_pos.y));
}

void GameObject::setPosX(float pos)
{
	m_pos.x = pos;
}

void GameObject::setPosY(float pos)
{
	m_pos.y = pos;
}

sf::Vector2f& GameObject::getVel() {
	return m_vel;
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

void GameObject::setNextPosX(float pos)
{
	m_nextPos.x = pos;
}

void GameObject::setNextPosY(float pos)
{
	m_nextPos.y = pos;
}

sf::Vector2f& GameObject::getNextVel()
{
	return m_nextVel;
}

void GameObject::setNextVel(sf::Vector2f pos)
{
	m_nextVel = pos;
}

void GameObject::setNextVelX(float pos)
{
	m_nextVel.x = pos;
}

void GameObject::setNextVelY(float pos)
{
	m_nextVel.y = pos;
}


sf::Vector2f GameObject::getSpritePos() {
	return m_sprite.getPosition();
}
