#include "GameObject.h"

// necessary because of forward declaration in GameObject.h
#include "game/object/extensions/MovementX.h"
#include "game/object/extensions/MultiJump.h"
#include "game/object/extensions/KickUp.h"
#include "game/object/extensions/Inventory.h"
#include "game/object/extensions/OrientCorrection.h"

int GameObject::Identifier_count = 0;
std::string GameObject::generateIdentifier(std::string name)
{
	Identifier_count++;
	return name + "#" + std::to_string(GameObject::Identifier_count);
}

GameObject::GameObject(std::string identifier, StringMapMap& setupMap, std::function<GameObject*(const std::string& identifier)> idGoMapper) :
	m_type(setupMap[Reader::DEFAULT_PARAGRAPH][S_TYPE]),
	m_identifier(identifier),
	m_idGoMapper(idGoMapper)
{
	initSetupMap(setupMap);
	applySetupMap(setupMap);
}

GameObject::GameObject(StringMapMap& setupMap, std::function<GameObject*(const std::string& identifier)> idGoMapper) :
	GameObject(generateIdentifier(setupMap[Reader::DEFAULT_PARAGRAPH][S_NAME]), setupMap, idGoMapper)
{}

GameObject::~GameObject() {
	Log::ger().log("destroy " + getIdentifier(), Log::Label::Default, GO_LOG);
	for(auto& e : m_extensions) delete e.second;
	m_extensions.clear();
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
		if(setupMap[S_HITBOX_PARAGRAPH][S_TYPE] == S_CIRCLE_FORM) {
			float radius = Helper::toFloat(setupMap[S_HITBOX_PARAGRAPH]["1"]);
			initBody(config, createCircleShape(radius, density));
		} else if(setupMap[S_HITBOX_PARAGRAPH][S_TYPE] == S_POLYGON_FORM) {
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
		if(Helper::toBool(setupMap[S_EXTENSIONS_PARAGRAPH][S_KICKUP_EXTENSION]))
			initExtension(S_KICKUP_EXTENSION, setupMap);
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
	if(extensionName == S_KICKUP_EXTENSION)
		m_extensions[S_KICKUP_EXTENSION] = new KickUp(this);
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

void GameObject::updateFlags() {
	const float tolerance = 0.35;
	m_isFalling = getVel().y > tolerance;
	m_isRising = getVel().y < -tolerance;
	m_isMoving = getVel().x != 0;
}

void GameObject::apply() {
	updateFlags();
}

void GameObject::update(sf::Time ellapsed) {
	for(auto& e : m_extensions) e.second->update(ellapsed);
}

void GameObject::event(Event ev) {
	for(auto& e : m_extensions) e.second->event(ev);
}

void GameObject::toggleLogging()
{
	m_log = !m_log;
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

sf::Color GameObject::getColor() const
{
	return m_color;
}

void GameObject::setColor(sf::Color s)
{
	m_color = s;
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

int GameObject::getZindex() const
{
	return m_zindex;
}

void GameObject::setZindex(int s)
{
	m_zindex = s;
}

bool GameObject::isMoving() const {
	return m_isMoving;
}

bool GameObject::isRising() const {
	return m_isRising;
}

bool GameObject::isFalling() const {
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

const Extension& GameObject::getExtension(const std::string& name) const
{
	return *m_extensions.at(name);
}

