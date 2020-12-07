#ifndef SWONE_GAME_OBJECT_GAMEOBJECT_H
#define SWONE_GAME_OBJECT_GAMEOBJECT_H

// Activate M_PI define
#define _USE_MATH_DEFINES
#include <cmath>
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

#define SPEED_FACTOR 0.5

#define JUMP_FORCE 16
#define MOVE_FORCE 7

// forward declaration to avoid dependency cyclus
class Extension;

class GameObject : public Handleable {
public:
	enum MovementAnimation : char { Up, Left, Right, Down, Steady };
	// gameobject property names
	// global
	static constexpr const char* GAMEOBJECT_TYPE_NAME = "type";
	static constexpr const char* GAMEOBJECT_PLAYER_TYPE = "player";
	static constexpr const char* GAMEOBJECT_OBJECT_TYPE = "object";

	static constexpr const char* GAMEOBJECT_NAME_NAME = "name";
	static constexpr const char* GAMEOBJECT_ID_NAME = "id";
	static constexpr const char* GAMEOBJECT_INITIAL_POS_NAME = "initial_position";
	static constexpr const char* GAMEOBJECT_VELOCITY_NAME = "velocity";
	static constexpr const char* GAMEOBJECT_COLOR_NAME = "color";
	static constexpr const char* GAMEOBJECT_TEXTURE_NAME = "texture";
	static constexpr const char* GAMEOBJECT_HITBOX_NAME = "hitbox";
	// animation paragraphs (keys are irrelevant)
	static constexpr const char* GAMEOBJECT_ANI_FRAME_TIME = "frame_time";
	static constexpr const char* GAMEOBJECT_ANI_UP_PARAGRAPH = "ani_up";
	static constexpr const char* GAMEOBJECT_ANI_LEFT_PARAGRAPH = "ani_left";
	static constexpr const char* GAMEOBJECT_ANI_RIGHT_PARAGRAPH = "ani_right";
	static constexpr const char* GAMEOBJECT_ANI_DOWN_PARAGRAPH = "ani_down";
	static constexpr const char* GAMEOBJECT_ANI_STEADY_PARAGRAPH = "ani_steady";
	// extensions
	static constexpr const char* GAMEOBJECT_EXTENSIONS_PARAGRAPH = "extensions";
	static constexpr const char* GAMEOBJECT_GRAVITY_EXTENSION = "gravity";
	static constexpr const char* GAMEOBJECT_MOVEMENTX_EXTENSION = "movementx";
	static constexpr const char* GAMEOBJECT_MULTIJUMP_EXTENSION = "multijump";
	static constexpr const char* GAMEOBJECT_INVENTORY_EXTENSION = "inventory";

	GameObject(std::map<std::string, StringMap>& setupMap);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void event(sf::Event& e);
	virtual void update(sf::Time ellapsed);

	// calculate the next position of the object and save the result in m_nextPos
	virtual void calculateVel(sf::Time ellapsed, float gravity);
	// calculate the next speed vct of the object and save the result in m_nextVec
	virtual void calculatePos(sf::Time ellapsed);
	// is called with the tiles which are hitting the corners
	// of the hitbox of the m_nextPos vector
	virtual void onTilesY(Tile, Tile);
	virtual void onTilesX(Tile, Tile);
	// if the object is complete out of map, this function is called
	virtual void onOutOfMap(Tile border);

	sf::Vector2f getHitboxLeftTop(const sf::Vector2f& pos);
	sf::Vector2f getHitboxRightTop(const sf::Vector2f& pos);
	sf::Vector2f getHitboxLeftBottom(const sf::Vector2f& pos);
	sf::Vector2f getHitboxRightBottom(const sf::Vector2f& pos);
	sf::FloatRect getHitboxBounds() const;
	AnimatedSprite* getAnimatedSprite();
	ph::Body* getBody() const;

	void toggleLogging();

	std::string getIdentifier() const;

	std::string getName();
	void setName(std::string s);

	sf::Color getColor();
	void setColor(sf::Color s);

	std::string getTexturePath();
	void setTexturePath(std::string s);

	sf::Vector2f getPossibleVel();
	void setPossibleVel(sf::Vector2f s);

	sf::Vector2f& getVel();
	void setVel(sf::Vector2f pos);
	void setVelX(float pos);
	void setVelY(float pos);

	sf::Vector2f& getPos();
	void setPos(sf::Vector2f pos);
	void setPosX(float pos);
	void setPosY(float pos);

	sf::Vector2f& getNextPos();
	void setNextPos(sf::Vector2f pos);
	void setNextPosX(float pos);
	void setNextPosY(float pos);

	sf::Vector2f& getNextVel();
	void setNextVel(sf::Vector2f pos);
	void setNextVelX(float pos);
	void setNextVelY(float pos);

	sf::FloatRect getHitbox();
	void setHitbox(sf::FloatRect);

	sf::Transformable getObjTransform() const;
	void setAngle(float s);
	void setScale(float s);

	bool isVisible();
	void setVisible(bool s);

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
	const std::string m_identifier;
	std::string m_name;

	sf::Color m_color;
	std::string m_texturePath;
	sf::Texture m_texture;
	Animation m_ani_up;
	Animation m_ani_left;
	Animation m_ani_right;
	Animation m_ani_down;
	Animation m_ani_steady;
	AnimatedSprite m_sprite;

	bool m_isRising = false; // movement to top
	bool m_isFalling = false; // movement to bottom
	bool m_isMoving = false;  // movement on x

	// was move/jump force
	sf::Vector2f m_possibleVel = sf::Vector2f(MOVE_FORCE, JUMP_FORCE);

	sf::Vector2f m_pos = sf::Vector2f(0, 0); // position in map
	sf::Vector2f m_vel = sf::Vector2f(0, 0); // direction velocity
	sf::Vector2f m_nextPos = sf::Vector2f(0, 0); // position in map after next calculation
	sf::Vector2f m_nextVel = sf::Vector2f(0, 0); // direction velocity after next calculation
	// TODO
	sf::FloatRect m_hitbox = sf::FloatRect(0.1875, 0.0, 0.1094, 0.5);

	ph::PolygonShape m_shape;
	ph::Body* m_body;

	// represents transformations relative to MapTiles,
	// not to MapPixel, the inherited Transformable is used for that..
	sf::Transformable m_objTransform;

	std::vector<Extension*> m_extensions;
};

#endif

