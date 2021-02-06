#include <client/game/object/GameObjectDrawing.h>

// necessary because of forward declaration
#include "client/game/object/ExtensionDrawing.h"
#include "client/game/object/extensions/InventoryDrawing.h"

GameObjectDrawing::GameObjectDrawing(const GameObject& obj, StringMapMap& setupMap) :
	m_obj(obj)
{
	initSetupMap(setupMap);
	applySetupMap(setupMap);
}

GameObjectDrawing::~GameObjectDrawing()
{
	for(auto& e : m_extensions) delete e.second;
	m_extensions.clear();
}

void GameObjectDrawing::initSetupMap(StringMapMap& setupMap)
{
	if(setupMap.count(GameObject::S_EXTENSIONS_PARAGRAPH)){
		if(Helper::toBool(setupMap[GameObject::S_EXTENSIONS_PARAGRAPH][GameObject::S_INVENTORY_EXTENSION])) {
			initExtension(GameObject::S_INVENTORY_EXTENSION, setupMap);
		}
	}
}

void GameObjectDrawing::applySetupMap(StringMapMap& setupMap)
{
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH)){
		auto& global = setupMap[Reader::DEFAULT_PARAGRAPH];
		if(global.count(GameObject::S_SCALE))
			setScale(Helper::toVector2f(global[GameObject::S_SCALE]));
	}
	if(setupMap.count(GameObject::S_ANI_UP_PARAGRAPH))
		setAnimationFrames(m_ani_up, setupMap[GameObject::S_ANI_UP_PARAGRAPH]);
	if(setupMap.count(GameObject::S_ANI_LEFT_PARAGRAPH))
		setAnimationFrames(m_ani_left, setupMap[GameObject::S_ANI_LEFT_PARAGRAPH]);
	if(setupMap.count(GameObject::S_ANI_RIGHT_PARAGRAPH))
		setAnimationFrames(m_ani_right, setupMap[GameObject::S_ANI_RIGHT_PARAGRAPH]);
	if(setupMap.count(GameObject::S_ANI_DOWN_PARAGRAPH))
		setAnimationFrames(m_ani_down, setupMap[GameObject::S_ANI_DOWN_PARAGRAPH]);
	if(setupMap.count(GameObject::S_ANI_STEADY_PARAGRAPH))
		setAnimationFrames(m_ani_steady, setupMap[GameObject::S_ANI_STEADY_PARAGRAPH]);

	for(auto& e : m_extensions) e.second->applyConfig(setupMap);
}

void GameObjectDrawing::applyConfig(GameObject::Config config)
{
	for(auto& e : config.extensionMap) {
		if(!m_extensions.count(e.first))
			initExtension(e.first, config.extensionMap);
	}
}

void GameObjectDrawing::initExtension(std::string extensionName, StringMapMap& setupMap)
{
	if(extensionName == GameObject::S_INVENTORY_EXTENSION)
		m_extensions[GameObject::S_INVENTORY_EXTENSION] = new InventoryDrawing(static_cast<const Inventory&>(m_obj.getExtension(GameObject::S_INVENTORY_EXTENSION)), setupMap);
}

void GameObjectDrawing::setAnimationFrames(Animation& animation, StringMap& m)
{
	if(m_obj.getTexturePath().empty())
		throw std::invalid_argument("GameObject Texture is missing.");
	if(m.count(GameObject::S_ANI_FRAME_TIME))
		animation.setFrameTime(sf::seconds(Helper::toFloat(m[GameObject::S_ANI_FRAME_TIME])));
	animation.setSpriteSheet(*GameReader::loadTexture(m_obj.getTexturePath()));
	for(int i = 1; m.count(std::to_string(i)); i++){
		animation.addFrame(Helper::toIntRect(m[std::to_string(i)]));
	}
}

void GameObjectDrawing::setMovementAnimationAutomatic()
{
	if(!m_movementAnimationAutomatic)
		return;
	if(m_obj.isMoving() && m_obj.getVel().x < 0)
		setMovementAnimation(MovementAnimation::Left);
	else if(m_obj.isMoving() && m_obj.getVel().x > 0)
		setMovementAnimation(MovementAnimation::Right);
	else if(m_obj.isFalling())
		setMovementAnimation(MovementAnimation::Down);
	else if(m_obj.isRising())
		setMovementAnimation(MovementAnimation::Up);
	else
		setMovementAnimation(MovementAnimation::Steady);
}

void GameObjectDrawing::setMovementAnimation(MovementAnimation ani) {
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

void GameObjectDrawing::playAnimationOnce(Animation& animation, std::function<void()> endCb)
{
	setMovementAnimationAutomatic(false, false);
	if(endCb == NULL)
		endCb = [&](){
			setMovementAnimationAutomatic(true, true);
		};
	m_sprite.setEndCallback(endCb);
	setAnimation(animation);
}

void GameObjectDrawing::setAnimation(Animation& animation)
{
	// use ani_steady if animation is not defined
	// else make invisible
	if(animation.getSpriteSheet() == NULL) {
		if(m_ani_steady.getSpriteSheet() == NULL) {
			m_drawable = false;
			return;
		}
		animation = m_ani_steady;
	}
	m_drawable = true;
	// sets animation automaticly if pointer changed
	m_sprite.play(animation);
	setOrigin(m_sprite.getLocalBounds().width/2,m_sprite.getLocalBounds().height/2);
}

bool GameObjectDrawing::isMovementAnimationAutomatic()
{
	return m_movementAnimationAutomatic;
}

void GameObjectDrawing::setMovementAnimationAutomatic(bool s, bool looped)
{
	m_movementAnimationAutomatic = s;
	m_sprite.setLooped(looped);
}

const GameObject& GameObjectDrawing::getGameObject() const
{
	return m_obj;
}

void GameObjectDrawing::update(sf::Time ellapsed)
{
	setPosition(Map::toMapPixelX(m_obj.getPos().x), Map::toMapPixelY(m_obj.getPos().y));
	setRotation(m_obj.getBody()->GetOrientAngle());
	m_sprite.setColor(m_obj.getColor());
	setMovementAnimationAutomatic();
	m_sprite.update(ellapsed);
	for(auto& e : m_extensions) e.second->update(ellapsed);
}

void GameObjectDrawing::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if(m_drawable) {
		states.transform *= getTransform();
		target.draw(m_sprite, states);
	}

	for(auto& e : m_extensions) e.second->draw(target, states);
}

