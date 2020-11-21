#ifndef SWONE_GAME_GAMEOBJECT_H
#define SWONE_GAME_GAMEOBJECT_H

// Activate M_PI define
#define _USE_MATH_DEFINES
#include <cmath>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Handleable.h"
#include "Map.h"
#include "graphics/Animation.h"
#include "graphics/AnimatedSprite.h"
#include "util/reader/Reader.h"
#include "util/Helper.h"

#define SPEED_FACTOR 0.5
#define DEFAULT_DRAG 100

#define JUMP_FORCE 16
#define MOVE_FORCE 7

// This scales the drag, so that with a Gravity of 25 and a
// drag property of 100, a terminal velocity of 50 is reached
#define SCALE_DRAG_CONST 0.0000000276f

class GameObject : public Handleable {
public:
	enum AnimationType : char { Up, Left, Right, Down, Steady };
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
	static constexpr const char* GAMEOBJECT_DRAG_NAME = "drag";
	// animation paragraphs (keys are irrelevant)
	static constexpr const char* GAMEOBJECT_ANI_UP_PARAGRAPH = "ani_up";
	static constexpr const char* GAMEOBJECT_ANI_LEFT_PARAGRAPH = "ani_left";
	static constexpr const char* GAMEOBJECT_ANI_RIGHT_PARAGRAPH = "ani_right";
	static constexpr const char* GAMEOBJECT_ANI_DOWN_PARAGRAPH = "ani_down";
	static constexpr const char* GAMEOBJECT_ANI_STEADY_PARAGRAPH = "ani_steady";

	GameObject(std::map<std::string, StringMap>& setupMap);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void event(sf::Event& e);
	virtual void update(sf::Time ellapsed);

	// TODO standard implementation of this functions
	// calculate the next position of the object and save the result in m_nextPos
	virtual sf::Vector2f& calculateVel(sf::Time ellapsed, float gravity);
	// calculate the next speed vct of the object and save the result in m_nextVec
	virtual sf::Vector2f& calculatePos(sf::Time ellapsed);
	// is called with the tiles which are hitting the corners
	// of the hitbox of the m_nextPos vector
	virtual void onTiles(MapTile leftTop, MapTile rightTop, MapTile leftBottom, MapTile rightBottom);
	// if the object is complete out of map, this function is called
	virtual void onOutOfMap();

	sf::Vector2f getHitboxLeftTop(const sf::Vector2f& pos);
	sf::Vector2f getHitboxRightTop(const sf::Vector2f& pos);
	sf::Vector2f getHitboxLeftBottom(const sf::Vector2f& pos);
	sf::Vector2f getHitboxRightBottom(const sf::Vector2f& pos);
	sf::FloatRect getHitboxBounds() const;
	AnimatedSprite* getAnimatedSprite();

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

	sf::Vector2f& getPos();
	void setPos(sf::Vector2f pos);

	sf::FloatRect getHitbox();
	void setHitbox(sf::FloatRect);

	float getDrag();
	void setDrag(float s);

	sf::Transformable getObjTransform() const;
	void setAngle(float s);
	void setScale(float s);

	bool isMoving();
	bool isFalling();
	bool isRising();
	// changes moving, falling, rising flags
	void updateFlags();
	// sets velocity of y to zero
	void stopFalling();

	void apply();
	void applyX();
	void applyY();

	sf::Vector2f getSpritePos();

	// sets animation depending on obj state
	void setAnimation();
	void setAnimation(AnimationType ani);

	// Write all the values that should be updated every tick into this function
	void updateLog() const;
protected:
	bool m_log = false;
	bool m_showHitbox = true;
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
	Animation* m_ani;
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

	// higher drag means slower falling
	float m_drag = DEFAULT_DRAG;
	// represents transformations relative to MapTiles,
	// not to MapPixel, the inherited Transformable is used for that..
	sf::Transformable m_objTransform;
private:
	void setAnimationFrames(Animation& animation, StringMap& frames);

	static float calculateDrag(const float drag, const float angle, const float speed);
};

#endif

