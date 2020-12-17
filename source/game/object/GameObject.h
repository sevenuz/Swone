#ifndef SWONE_GAME_OBJECT_GAMEOBJECT_H
#define SWONE_GAME_OBJECT_GAMEOBJECT_H

#include <vector>
#include <functional>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Handleable.h"
#include "game/Map.h"
#include "graphics/Animation.h"
#include "graphics/AnimatedSprite.h"
#include "util/reader/Reader.h"
#include "util/Helper.h"
#include "physics/Body.h"
#include "physics/Shape.h"
#include "physics/Manifold.h"
#include "physics/Scene.h"

#define SPEED_FACTOR 0.5

#define JUMP_FORCE 16
#define MOVE_FORCE 7

// forward declaration to avoid dependency cyclus
class Extension;

class GameObject : public Handleable, public ph::Body::Callback {
public:
	enum MovementAnimation : char { Up, Left, Right, Down, Steady };
	// S_ for Settings :)
	// gameobject property names
	// global
	static constexpr const char* S_TYPE = "type";
	static constexpr const char* S_PLAYER_TYPE = "player";
	static constexpr const char* S_OBJECT_TYPE = "object";

	static constexpr const char* S_NAME = "name";
	static constexpr const char* S_ID = "id";
	static constexpr const char* S_COLOR = "color";
	static constexpr const char* S_TEXTURE = "texture";
	static constexpr const char* S_SCALE = "scale";
	static constexpr const char* S_ZINDEX = "z-index";

	static constexpr const char* S_HITBOX_PARAGRAPH = "hitbox";
	// Shape
	static constexpr const char* S_DENSITY = "density";
	static constexpr const char* S_CIRCLE_TYPE = "circle";
	static constexpr const char* S_POLYGON_TYPE = "polygon";
	// Body
	static constexpr const char* S_INITIAL_POS = "initial_position";
	static constexpr const char* S_SOLID = "solid";
	static constexpr const char* S_ROTATABLE = "rotatable";
	static constexpr const char* S_COLLIDABLE_SOLID = "collidableSolid";
	static constexpr const char* S_COLLIDABLE_UNSOLID = "collidableUnsolid";
	static constexpr const char* S_FRICTION = "friction";
	static constexpr const char* S_RESTITUTION = "restitution";
	static constexpr const char* S_ORIENT = "orient";

	// animation paragraphs
	static constexpr const char* S_ANI_FRAME_TIME = "frame_time";
	static constexpr const char* S_ANI_UP_PARAGRAPH = "ani_up";
	static constexpr const char* S_ANI_LEFT_PARAGRAPH = "ani_left";
	static constexpr const char* S_ANI_RIGHT_PARAGRAPH = "ani_right";
	static constexpr const char* S_ANI_DOWN_PARAGRAPH = "ani_down";
	static constexpr const char* S_ANI_STEADY_PARAGRAPH = "ani_steady";
	// extensions
	static constexpr const char* S_EXTENSIONS_PARAGRAPH = "extensions";
	static constexpr const char* S_GRAVITY_EXTENSION = "gravity";
	static constexpr const char* S_MOVEMENTX_EXTENSION = "movementx";
	static constexpr const char* S_MULTIJUMP_EXTENSION = "multijump";
	static constexpr const char* S_INVENTORY_EXTENSION = "inventory";
	static constexpr const char* S_ORIENT_CORRECTION_EXTENSION = "orientCorrection";

	GameObject(std::map<std::string, StringMap>& setupMap);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void event(sf::Event& e);
	virtual void update(sf::Time ellapsed);

	virtual void onObjectCollision(ph::Manifold* manifold, GameObject* go);
	virtual void onTileCollision(ph::Manifold* manifold, Tile* t);

	void onCollision(ph::Manifold* manifold) override;
	const std::string getType() const override;

	static GameObject* castBodyCallback(ph::Body::Callback* c);

	void toggleLogging();

	AnimatedSprite* getAnimatedSprite();
	sf::Vector2f getSpriteScaleTo(sf::Vector2f v);
	ph::Body* getBody() const;

	const std::string getIdentifier() const;

	std::string getName();
	void setName(std::string s);

	sf::Color getColor();
	void setColor(sf::Color s);

	std::string getTexturePath();
	void setTexturePath(std::string s);

	const sf::Vector2f getVel() const;
	void setVel(sf::Vector2f pos);
	void setVelX(float pos);
	void setVelY(float pos);

	const sf::Vector2f getPos() const;
	void setPos(sf::Vector2f pos);
	void setPosX(float pos);
	void setPosY(float pos);

	bool isVisible();
	void setVisible(bool s);

	int getZindex();
	void setZindex(int s);

	bool isMovementAnimationAutomatic();
	void setMovementAnimationAutomatic(bool s, bool looped = true);

	bool isMoving();
	bool isFalling();
	bool isRising();
	// changes moving, falling, rising flags
	void updateFlags();

	void apply();

	sf::Vector2f getSpritePos();

	// sets animation depending on obj state
	void setMovementAnimationAutomatic();
	void setMovementAnimation(MovementAnimation ani);
	void setAnimation(Animation& animation);
	void playAnimationOnce(Animation& animation, std::function<void()> endCb = NULL);
	void setAnimationFrames(Animation& animation, StringMap& frames);

	// Write all the values that should be updated every tick into this function
	void updateLog() const;
private:
	bool m_log = false;
	bool m_visible = true;
	bool m_movementAnimationAutomatic = true;
	const std::string m_type;
	const std::string m_identifier;
	std::string m_name;

	sf::Color m_color;
	std::string m_texturePath;
	Animation m_ani_up;
	Animation m_ani_left;
	Animation m_ani_right;
	Animation m_ani_down;
	Animation m_ani_steady;
	AnimatedSprite m_sprite;

	bool m_isRising = false; // movement to top
	bool m_isFalling = false; // movement to bottom
	bool m_isMoving = false;  // movement on x

	int m_zindex = 1;

	sf::Vector2f m_possibleVel = sf::Vector2f(MOVE_FORCE, JUMP_FORCE);
	sf::Vector2f m_startPos = sf::Vector2f(0.0f, 0.0f);

	ph::Body* m_body = NULL;

	std::vector<Extension*> m_extensions;
};

#endif

