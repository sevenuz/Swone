#include "GameObject.h"

// necessary because of forward declaration in GameObject.h
#include "game/object/extensions/MovementX.h"
#include "game/object/extensions/MultiJump.h"
#include "game/object/extensions/Inventory.h"
#include "game/object/extensions/OrientCorrection.h"

// TODO generate Id of GO differently
int GameObject::Identifier_count = 0;
std::string GameObject::generateIdentifier(std::string name)
{
	Identifier_count++;
	return name + "#" + std::to_string(GameObject::Identifier_count);
}

GameObject::GameObject(std::string type, GameObject::Config config, ph::Shape* shape)
	: m_type(type),
		m_identifier(generateIdentifier(config.name))
{
	initBody(config.body, shape);
	applyConfig(config);
}

GameObject::GameObject(std::string type, Config config, std::vector<ph::Vec2>& vertices, float density)
	: GameObject(type, config, createPolyShape(vertices, density))
{}

GameObject::GameObject(std::string type, Config config, float radius, float density)
	: GameObject(type, config, createCircleShape(radius, density))
{}

GameObject::GameObject(StringMapMap& setupMap)
	: m_type(setupMap[Reader::DEFAULT_PARAGRAPH][S_TYPE]),
		m_identifier(generateIdentifier(setupMap[Reader::DEFAULT_PARAGRAPH][S_NAME]))
{
	initSetupMap(setupMap);
	applySetupMap(setupMap);
}

GameObject::~GameObject() {
	Log::ger().log("destroy " + getIdentifier());
}

ph::Shape* GameObject::createPolyShape(std::vector<ph::Vec2>& vertices, float density)
{
	ph::PolygonShape* poly = new ph::PolygonShape(density);
	poly->Set(vertices.data(), vertices.size());
	return poly;
}

ph::Shape* GameObject::createCircleShape(float radius, float density)
{
	assert(radius);
	return new ph::Circle(radius, density);
}

void GameObject::initBody(ph::Body::Config config, ph::Shape* shape)
{
	if(shape == NULL) {
		// GameObject Default Shape
		Log::ger().log("Default Hitbox", Log::Label::Warning, GO_LOG);
		ph::PolygonShape* poly = new ph::PolygonShape();
		poly->SetBox(0.0f, 0.0f);
		shape = poly;
	}
	m_body = new ph::Body(config, shape, this);
}

void GameObject::initSetupMap(StringMapMap& setupMap)
{
	bool hasHitbox = setupMap.count(S_HITBOX_PARAGRAPH);
	// hasHitbox - Flag: collidableSolid, collidableUnsolid, rotatable, solid, skip
	ph::Body::Config config{hasHitbox, hasHitbox, hasHitbox, !hasHitbox, !hasHitbox};
	if(hasHitbox) {
		float density = Helper::toFloat(setupMap[S_HITBOX_PARAGRAPH][S_DENSITY], 1);
		if(setupMap[S_HITBOX_PARAGRAPH][S_TYPE] == S_CIRCLE_TYPE) {
			float radius = Helper::toFloat(setupMap[S_HITBOX_PARAGRAPH]["1"]);
			initBody(config, createCircleShape(radius, density));
		} else if(setupMap[S_HITBOX_PARAGRAPH][S_TYPE] == S_POLYGON_TYPE) {
			std::vector<ph::Vec2> vertices;
			for(int i = 1; setupMap[S_HITBOX_PARAGRAPH].count(std::to_string(i)); i++){
				sf::Vector2f v = Helper::toVector2f(setupMap[S_HITBOX_PARAGRAPH][std::to_string(i)]);
				vertices.push_back(ph::Vec2(v.x, v.y));
			}
			initBody(config, createPolyShape(vertices, density));
		} else {
			hasHitbox = false;
		}
	} else {
		Log::ger().log(m_identifier + " has no hitbox paragraph.", Log::Label::Warning, GO_LOG);
	}
	if(!hasHitbox)
		initBody(config, NULL); // Default Hitbox

	if(setupMap.count(S_EXTENSIONS_PARAGRAPH)){
		if(Helper::toBool(setupMap[S_EXTENSIONS_PARAGRAPH][S_MOVEMENTX_EXTENSION]))
			initExtension(S_MOVEMENTX_EXTENSION, setupMap);
		if(Helper::toBool(setupMap[S_EXTENSIONS_PARAGRAPH][S_MULTIJUMP_EXTENSION]))
			initExtension(S_MULTIJUMP_EXTENSION, setupMap);
		if(Helper::toBool(setupMap[S_EXTENSIONS_PARAGRAPH][S_INVENTORY_EXTENSION]))
			initExtension(S_INVENTORY_EXTENSION, setupMap);
		if(Helper::toBool(setupMap[S_EXTENSIONS_PARAGRAPH][S_ORIENT_CORRECTION_EXTENSION]))
			initExtension(S_ORIENT_CORRECTION_EXTENSION, setupMap);
	}
}

void GameObject::initExtension(std::string extensionName, StringMapMap& setupMap)
{
	if(extensionName == S_MOVEMENTX_EXTENSION)
		m_extensions[S_MOVEMENTX_EXTENSION] = new MovementX(this, setupMap);
	if(extensionName == S_MULTIJUMP_EXTENSION)
		m_extensions[S_MULTIJUMP_EXTENSION] = new MultiJump(this, setupMap);
	if(extensionName == S_INVENTORY_EXTENSION)
		m_extensions[S_INVENTORY_EXTENSION] = new Inventory(this, setupMap);
	if(extensionName == S_ORIENT_CORRECTION_EXTENSION)
		m_extensions[S_ORIENT_CORRECTION_EXTENSION] = new OrientCorrection(this);
}

void GameObject::applySetupMap(StringMapMap& setupMap)
{
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH)){
		auto& global = setupMap[Reader::DEFAULT_PARAGRAPH];
		if(global.count(S_NAME))
			setName(global[S_NAME]);
		if(global.count(S_COLOR))
			setColor(Helper::toColor(global[S_COLOR]));
		if(global.count(S_TEXTURE))
			setTexturePath(global[S_TEXTURE]);
		if(global.count(S_SCALE))
			setScale(Helper::toVector2f(global[S_SCALE]));
		if(global.count(S_ZINDEX))
			setZindex(Helper::toInt(global[S_ZINDEX]));
	}

	if(setupMap.count(S_HITBOX_PARAGRAPH)){
		ph::Body::Config config = m_body->GetConfig();

		if(setupMap[S_HITBOX_PARAGRAPH].count(S_INITIAL_POS)) {
			sf::Vector2f f = Helper::toVector2f(setupMap[S_HITBOX_PARAGRAPH][S_INITIAL_POS]);
			config.x = f.x;
			config.y = f.y;
		}
		if(setupMap[S_HITBOX_PARAGRAPH].count(S_SOLID))
			config.solid = Helper::toBool(setupMap[S_HITBOX_PARAGRAPH][S_SOLID]);
		if(setupMap[S_HITBOX_PARAGRAPH].count(S_ROTATABLE))
			config.rotatable = Helper::toBool(setupMap[S_HITBOX_PARAGRAPH][S_ROTATABLE]);
		if(setupMap[S_HITBOX_PARAGRAPH].count(S_COLLIDABLE_SOLID))
			config.collidableSolid = Helper::toBool(setupMap[S_HITBOX_PARAGRAPH][S_COLLIDABLE_SOLID]);
		if(setupMap[S_HITBOX_PARAGRAPH].count(S_COLLIDABLE_UNSOLID))
			config.collidableUnsolid = Helper::toBool(setupMap[S_HITBOX_PARAGRAPH][S_COLLIDABLE_UNSOLID]);

		if(setupMap[S_HITBOX_PARAGRAPH].count(S_FRICTION)) {
			sf::Vector2f f = Helper::toVector2f(setupMap[S_HITBOX_PARAGRAPH][S_FRICTION]);
			config.staticFriction = f.x;
			config.dynamicFriction = f.y;
		}
		if(setupMap[S_HITBOX_PARAGRAPH].count(S_RESTITUTION))
			config.restitution = Helper::toFloat(setupMap[S_HITBOX_PARAGRAPH][S_RESTITUTION]);
		if(setupMap[S_HITBOX_PARAGRAPH].count(S_ORIENT))
			config.orient = (Helper::toFloat(setupMap[S_HITBOX_PARAGRAPH][S_ORIENT]))*ph::PI/180;

		m_body->ApplyConfig(config);
	}

	if(setupMap.count(S_ANI_UP_PARAGRAPH))
		setAnimationFrames(m_ani_up, setupMap[S_ANI_UP_PARAGRAPH]);
	if(setupMap.count(S_ANI_LEFT_PARAGRAPH))
		setAnimationFrames(m_ani_left, setupMap[S_ANI_LEFT_PARAGRAPH]);
	if(setupMap.count(S_ANI_RIGHT_PARAGRAPH))
		setAnimationFrames(m_ani_right, setupMap[S_ANI_RIGHT_PARAGRAPH]);
	if(setupMap.count(S_ANI_DOWN_PARAGRAPH))
		setAnimationFrames(m_ani_down, setupMap[S_ANI_DOWN_PARAGRAPH]);
	if(setupMap.count(S_ANI_STEADY_PARAGRAPH))
		setAnimationFrames(m_ani_steady, setupMap[S_ANI_STEADY_PARAGRAPH]);

	for(auto& e : m_extensions) e.second->applyConfig(setupMap);
}

void GameObject::applyConfig(GameObject::Config config)
{
	setName(config.name);
	setVisible(config.visible);
	setColor(config.color);
	setZindex(config.zindex);
	m_body->ApplyConfig(config.body);

	for(auto& e : config.extensionMap) {
		if(!m_extensions.count(e.first))
			initExtension(e.first, config.extensionMap);
		else
			m_extensions[e.first]->applyConfig(config.extensionMap);
	}
}

GameObject::Config GameObject::getConfig()
{
	GameObject::Config config;
	config.name = getName();
	config.visible = isVisible();
	config.color = getColor();
	config.zindex = getZindex();
	config.body = m_body->GetConfig();

	for(auto& e : m_extensions) e.second->getConfig(config.extensionMap);

	return config;
}

GameObject* GameObject::castBodyCallback(ph::Body::Callback* c)
{
	if(c->getCallbackType() == S_OBJECT_TYPE)
		return static_cast<GameObject*>(c);
	else
		return NULL;
}

void GameObject::onCollision(ph::Manifold* manifold)
{
	GameObject* g = castBodyCallback(manifold->A->callback);
	GameObject* g2 = castBodyCallback(manifold->B->callback);
	Tile* t = Tile::castBodyCallback(manifold->A->callback);
	Tile* t2 = Tile::castBodyCallback(manifold->B->callback);
	// t && t2 not possible, at least one is this
	if(g != NULL && g2 != NULL) {
		onObjectCollision(manifold, g == this ? g2 : g);
	} else if(g != NULL && t2 != NULL) {
		onTileCollision(manifold, t2);
	} else if(t != NULL && g2 != NULL) {
		onTileCollision(manifold, t);
	} else {
		Log::ger().log("Unhandled Collisions in GameObject", Log::Label::Warning, GO_LOG);
	}
}

void GameObject::onObjectCollision(ph::Manifold* manifold, GameObject* go)
{
	for(auto& e : m_extensions) e.second->onObjectCollision(manifold, go);
}

void GameObject::onTileCollision(ph::Manifold* manifold, Tile* t)
{
	for(auto& e : m_extensions) e.second->onTileCollision(manifold, t);
}

void GameObject::setAnimationFrames(Animation& animation, StringMap& m)
{
	if(m_texturePath.empty())
		throw std::invalid_argument("GameObject Texture is missing.");
	if(m.count(S_ANI_FRAME_TIME))
		animation.setFrameTime(sf::seconds(Helper::toFloat(m[S_ANI_FRAME_TIME])));
	animation.setSpriteSheet(*Helper::loadTexture(m_texturePath));
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
	// use ani_steady if animation is not defined
	// else make invisible
	if(animation.getSpriteSheet() == NULL) {
		if(m_ani_steady.getSpriteSheet() == NULL) {
			setVisible(false);
			return;
		}
		animation = m_ani_steady;
	}
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
	for(auto& e : m_extensions) e.second->update(ellapsed);
}

void GameObject::event(sf::Event& ev) {
	for(auto& e : m_extensions) e.second->event(ev);
}

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if(m_visible) {
		states.transform *= getTransform();
		target.draw(m_sprite, states);
	}

	for(auto& e : m_extensions) e.second->draw(target, states);
}

void GameObject::toggleLogging()
{
	m_log = !m_log;
	if(m_log) {
		Log::ger().detailsPutValue(m_sprite.getTexture(), "gameObject_texture", m_identifier);
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

	for(auto& e : m_extensions) e.second->updateLog();
}

const std::string GameObject::getCallbackType() const {
	return S_OBJECT_TYPE;
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
}

AnimatedSprite* GameObject::getAnimatedSprite() {
	return &m_sprite;
}

sf::Vector2f GameObject::getSpriteScaleTo(sf::Vector2f v)
{
	return sf::Vector2f(
		v.x / getAnimatedSprite()->getLocalBounds().width,
		v.y / getAnimatedSprite()->getLocalBounds().height
	);
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

int GameObject::getZindex()
{
	return m_zindex;
}

void GameObject::setZindex(int s)
{
	m_zindex = s;
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
