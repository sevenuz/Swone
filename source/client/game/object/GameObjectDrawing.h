#ifndef SWONE_CLIENT_GAME_OBJECT_DRAWING_H
#define SWONE_CLIENT_GAME_OBJECT_DRAWING_H

#include <functional>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include "graphics/Animation.h"
#include "graphics/AnimatedSprite.h"

#include "Updateable.h"

#include "game/object/GameObject.h"

class ExtensionDrawing;

class GameObjectDrawing : public Updateable, public sf::Drawable, public sf::Transformable {
public:
	enum MovementAnimation : char { Up, Left, Right, Down, Steady };

	GameObjectDrawing(const GameObject& go, StringMapMap& setupMap, std::function<GameObjectDrawing*(GameObject*)> goGodMapper);
	virtual ~GameObjectDrawing();

	void applySetupMap(StringMapMap& setupMap);
	void applyConfig(GameObject::Config config);

	// sets animation depending on obj state
	void setMovementAnimationAutomatic();
	void setMovementAnimation(MovementAnimation ani);
	void setAnimation(Animation& animation);
	void playAnimationOnce(Animation& animation, std::function<void()> endCb = NULL);
	void setAnimationFrames(Animation& animation, StringMap& frames);

	sf::Vector2f getSpriteScaleTo(sf::Vector2f v);

	bool isMovementAnimationAutomatic();
	void setMovementAnimationAutomatic(bool s, bool looped = true);

	std::string getTexturePath() const;
	void setTexturePath(std::string s);

	const AnimatedSprite& getAnimatedSprite() const;
	const GameObject& getGameObject() const;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update(sf::Time elapsed);
private:
	void initSetupMap(StringMapMap& setupMap);
	void initExtension(std::string extensionName, StringMapMap& setupMap);

	std::function<GameObjectDrawing*(GameObject*)> m_goGodMapper;

	const GameObject& m_obj;
	std::map<std::string, ExtensionDrawing*> m_extensions;

	std::string m_texturePath;

	Animation m_ani_up;
	Animation m_ani_left;
	Animation m_ani_right;
	Animation m_ani_down;
	Animation m_ani_steady;
	AnimatedSprite m_sprite;

	bool m_movementAnimationAutomatic = true;
	bool m_drawable = false;
};

#endif
