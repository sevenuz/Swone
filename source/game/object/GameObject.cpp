#include "GameObject.h"

// necessary because of forward declaration in GameObject.h
#include "game/object/extensions/MovementX.h"
#include "game/object/extensions/MultiJump.h"
#include "game/object/extensions/Inventory.h"
#include "game/object/extensions/OrientCorrection.h"

GameObject::GameObject(std::map<std::string, StringMap>& setupMap)
	: m_type(setupMap[Reader::DEFAULT_PARAGRAPH][S_TYPE]),
		m_identifier(setupMap[Reader::DEFAULT_PARAGRAPH][S_ID])
{
	// TODO remove loop?
	for(auto& s: setupMap[Reader::DEFAULT_PARAGRAPH]){
		const std::string k = s.first;
		const std::string v = s.second;
		if(k == S_NAME)
			setName(v);
		else if(k == S_INITIAL_POS)
			setStartPos(Helper::toVector2f(v));
		else if(k == S_VELOCITY)
			setPossibleVel(Helper::toVector2f(v));
		else if(k == S_COLOR)
			setColor(Helper::toColor(v));
		else if(k == S_TEXTURE)
			setTexturePath(v);
	}

	bool hasHitbox = setupMap.count(S_HITBOX_PARAGRAPH);
	// if custom hitbox not exists, obj is static: not collidable, movable, rotatable
	ph::Body::Config config{NULL, m_startPos.x, m_startPos.y, this, hasHitbox, hasHitbox, hasHitbox};
	if(hasHitbox){
		float density = Helper::toFloat(setupMap[S_HITBOX_PARAGRAPH][S_DENSITY]);
		if(setupMap[S_HITBOX_PARAGRAPH][S_TYPE] == S_CIRCLE_TYPE) {
			// radius of the circle has the key "1"
			config.shape = new ph::Circle(Helper::toFloat(setupMap[S_HITBOX_PARAGRAPH]["1"]), density);
		} else if(setupMap[S_HITBOX_PARAGRAPH][S_TYPE] == S_POLYGON_TYPE) {
			ph::PolygonShape* poly = new ph::PolygonShape(density);
			std::vector<ph::Vec2> vertices;
			for(int i = 1; setupMap[S_HITBOX_PARAGRAPH].count(std::to_string(i)); i++){
				sf::Vector2f v = Helper::toVector2f(setupMap[S_HITBOX_PARAGRAPH][std::to_string(i)]);
				vertices.push_back(ph::Vec2(v.x, v.y));
			}
			poly->Set(vertices.data(), vertices.size());
			config.shape = poly;
		} else {
			Log::ger().log("Object has no hitbox type={circle|polygon}", Log::Label::Warning);
			hasHitbox = false;
		}

		if(setupMap[S_HITBOX_PARAGRAPH].count(S_MOVABLE))
			config.movable = Helper::toBool(setupMap[S_HITBOX_PARAGRAPH][S_MOVABLE]);
		if(setupMap[S_HITBOX_PARAGRAPH].count(S_ROTATABLE))
			config.rotatable = Helper::toBool(setupMap[S_HITBOX_PARAGRAPH][S_ROTATABLE]);
		if(setupMap[S_HITBOX_PARAGRAPH].count(S_COLLIDABLE))
			config.collidable = Helper::toBool(setupMap[S_HITBOX_PARAGRAPH][S_COLLIDABLE]);

		if(setupMap[S_HITBOX_PARAGRAPH].count(S_FRICTION)) {
			sf::Vector2f f = Helper::toVector2f(setupMap[S_HITBOX_PARAGRAPH][S_FRICTION]);
			config.staticFriction = f.x;
			config.dynamicFriction = f.y;
		}
		if(setupMap[S_HITBOX_PARAGRAPH].count(S_RESTITUTION))
			config.restitution = Helper::toFloat(setupMap[S_HITBOX_PARAGRAPH][S_RESTITUTION]);
		if(setupMap[S_HITBOX_PARAGRAPH].count(S_ORIENT))
			config.orient = (Helper::toFloat(setupMap[S_HITBOX_PARAGRAPH][S_ORIENT]))*ph::PI/180;
	}
	if(!hasHitbox) {
		// GameObject Default Shape
		Log::ger().log("Default Hitbox", Log::Label::Warning);
		ph::PolygonShape* poly = new ph::PolygonShape();
		poly->SetBox(0.1f, 0.1f);
		config.shape = poly;
	}
	m_body = new ph::Body(config);

	for(auto& p: setupMap){
		std::string paragraph = p.first;
		if(paragraph == S_ANI_UP_PARAGRAPH) {
			setAnimationFrames(m_ani_up, p.second);
		} else if(paragraph == S_ANI_LEFT_PARAGRAPH) {
			setAnimationFrames(m_ani_left, p.second);
		} else if(paragraph == S_ANI_RIGHT_PARAGRAPH) {
			setAnimationFrames(m_ani_right, p.second);
		} else if(paragraph == S_ANI_DOWN_PARAGRAPH) {
			setAnimationFrames(m_ani_down, p.second);
		} else if(paragraph == S_ANI_STEADY_PARAGRAPH) {
			setAnimationFrames(m_ani_steady, p.second);
		}
	}
	if(setupMap.count(S_EXTENSIONS_PARAGRAPH)){
		if(Helper::toBool(setupMap[S_EXTENSIONS_PARAGRAPH][S_MOVEMENTX_EXTENSION]))
			m_extensions.push_back(new MovementX(this, setupMap));
		if(Helper::toBool(setupMap[S_EXTENSIONS_PARAGRAPH][S_MULTIJUMP_EXTENSION]))
			m_extensions.push_back(new MultiJump(this, setupMap));
		if(Helper::toBool(setupMap[S_EXTENSIONS_PARAGRAPH][S_INVENTORY_EXTENSION]))
			m_extensions.push_back(new Inventory(this, setupMap));
		if(Helper::toBool(setupMap[S_EXTENSIONS_PARAGRAPH][S_ORIENT_CORRECTION_EXTENSION]))
			m_extensions.push_back(new OrientCorrection(this));
	}
}

GameObject* GameObject::castBodyCallback(ph::Body::Callback* c)
{
	if(c->getType() == S_OBJECT_TYPE || c->getType() == S_PLAYER_TYPE)
		return static_cast<GameObject*>(c);
	else
		return NULL;
}

void GameObject::onCollision(ph::Manifold* manifold)
{
	GameObject* g = castBodyCallback(manifold->A->cb);
	GameObject* g2 = castBodyCallback(manifold->B->cb);
	Tile* t = Tile::castBodyCallback(manifold->A->cb);
	Tile* t2 = Tile::castBodyCallback(manifold->B->cb);
	// t && t2 not possible, at least one is this
	if(g != NULL && g2 != NULL) {
		onObjectCollision(manifold, g == this ? g2 : g);
	} else if(g != NULL && t2 != NULL) {
		onTileCollision(manifold, t2);
	} else if(t != NULL && g2 != NULL) {
		onTileCollision(manifold, t);
	} else {
		Log::ger().log("Unhandled Collisions in GameObject", Log::Label::Warning);
	}
}

void GameObject::onObjectCollision(ph::Manifold* manifold, GameObject* go)
{
	for(Extension* e : m_extensions) e->onObjectCollision(manifold, go);
}

void GameObject::onTileCollision(ph::Manifold* manifold, Tile* t)
{
	for(Extension* e : m_extensions) e->onTileCollision(manifold, t);
}

void GameObject::setAnimationFrames(Animation& animation, StringMap& m)
{
	// TODO static map of loaded textures, this fn to Helper.h ?
	if(m_texturePath.empty())
		throw std::invalid_argument("GameObject Texture is missing.");
	if(m.count(S_ANI_FRAME_TIME))
		animation.setFrameTime(sf::seconds(Helper::toFloat(m[S_ANI_FRAME_TIME])));
	animation.setSpriteSheet(m_texture);
	for(int i = 1; m.count(std::to_string(i)); i++){
		animation.addFrame(Helper::toIntRect(m[std::to_string(i)]));
	}
}

void GameObject::setMovementAnimationAutomatic()
{
	if(!m_movementAnimationAutomatic)
		return;
	if(isMoving() && getVel().x < 0)
		setMovementAnimation(MovementAnimation::Left);
	else if(isMoving() && getVel().x > 0)
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
	setOrigin(m_sprite.getLocalBounds().width/2,m_sprite.getLocalBounds().height/2);
}

void GameObject::updateFlags() {
	const float tolerance = 0.35;
	m_isFalling = getVel().y > tolerance;
	m_isRising = getVel().y < -tolerance;
	m_isMoving = getVel().x != 0;
}

void GameObject::apply() {
	setPosition(Map::toMapPixelX(getPos().x), Map::toMapPixelY(getPos().y));
	setRotation(m_body->GetOrientAngle());
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

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if(m_visible) {
		states.transform *= getTransform();
		target.draw(m_sprite, states);
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
	log.detailsUpdateValue(std::to_string(getPos().x), "current_x", m_identifier);
	log.detailsUpdateValue(std::to_string(getPos().y), "current_y", m_identifier);
	log.detailsUpdateValue(std::to_string(getVel().x), "vel_x", m_identifier);
	log.detailsUpdateValue(std::to_string(getVel().y), "vel_y", m_identifier);

	for(Extension* e : m_extensions) e->updateLog();
}

const std::string GameObject::getType() const {
	return m_type;
}

const std::string GameObject::getIdentifier() const {
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

sf::Vector2f GameObject::getStartPos()
{
	return m_startPos;
}

void GameObject::setStartPos(sf::Vector2f s)
{
	m_startPos = s;
}

AnimatedSprite* GameObject::getAnimatedSprite() {
	return &m_sprite;
}

ph::Body* GameObject::getBody() const
{
	return m_body;
}

bool GameObject::isVisible()
{
	return m_visible;
}

void GameObject::setVisible(bool s)
{
	m_visible = s;
}

bool GameObject::isCollidable()
{
	return m_body->collidable;
}

void GameObject::setCollidable(bool s)
{
	m_body->collidable = s;
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

const sf::Vector2f GameObject::getPos() const {
	return Helper::toSfVec(m_body->position);
}

void GameObject::setPos(sf::Vector2f pos) {
	m_body->position = Helper::toPhVec(pos);
}

void GameObject::setPosX(float pos)
{
	m_body->position.x = pos;
}

void GameObject::setPosY(float pos)
{
	m_body->position.y = pos;
}

const sf::Vector2f GameObject::getVel() const {
	return Helper::toSfVec(m_body->velocity);
}

void GameObject::setVel(sf::Vector2f vel) {
	m_body->velocity = Helper::toPhVec(vel);
}

void GameObject::setVelX(float pos)
{
	m_body->velocity.x = pos;
}

void GameObject::setVelY(float pos)
{
	m_body->velocity.y = pos;
}

sf::Vector2f GameObject::getSpritePos() {
	return m_sprite.getPosition();
}
