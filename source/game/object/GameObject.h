#ifndef SWONE_GAME_OBJECT_GAMEOBJECT_H
#define SWONE_GAME_OBJECT_GAMEOBJECT_H

#include <vector>
#include <functional>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Updateable.h"
#include "game/Map.h"
#include "game/GameReader.h"
#include "util/reader/Reader.h"
#include "util/Helper.h"
#include "physics/Body.h"
#include "physics/Shape.h"
#include "physics/Manifold.h"
#include "physics/Scene.h"

#define GO_LOG false

#define SPEED_FACTOR 0.5

#define JUMP_FORCE 16
#define MOVE_FORCE 7

// forward declaration to avoid dependency cyclus
class Extension;
class GameObjectDrawing;

class GameObject : public Updateable, public ph::Body::Callback {
	friend Extension;
	friend GameObjectDrawing;
public:
	struct Event {
		bool left = false;
		bool right = false;
		bool up = false;
		bool down = false;
		bool action1 = false;
		bool action2 = false;
		bool action3 = false;
		bool action4 = false;
	};

	// S_ for Settings :)
	// gameobject property names
	// global
	static constexpr const char* S_TYPE = "type";
	static constexpr const char* S_PLAYER_TYPE = "player";
	static constexpr const char* S_OBJECT_TYPE = "object";
	static constexpr const char* S_ITEM_TYPE = "item";
	static constexpr const char* S_STATIC_TYPE = "static"; // these objects are not synced with the server

	static constexpr const char* S_NAME = "name";
	static constexpr const char* S_COLOR = "color";
	static constexpr const char* S_TEXTURE = "texture";
	static constexpr const char* S_TEXTURE_PATH = "texturePath"; // is set in Scenery
	static constexpr const char* S_SCALE = "scale";
	static constexpr const char* S_ZINDEX = "z-index";

	static constexpr const char* S_HITBOX_PARAGRAPH = "hitbox";
	// Shape
	static constexpr const char* S_DENSITY = "density";
	static constexpr const char* S_CIRCLE_FORM = "circle";
	static constexpr const char* S_POLYGON_FORM = "polygon";
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

	static int Identifier_count;
	static std::string generateIdentifier(std::string name);

	struct Config {
		std::string name = S_OBJECT_TYPE;

		bool visible = true;
		sf::Color color = sf::Color::Transparent;
		int zindex = 1;

		ph::Body::Config body;
		StringMapMap extensionMap; // TODO improve data exchange with extensions
	};

	GameObject(StringMapMap& setupMap, std::function<GameObject*(const std::string& identifier)> idGoMapper);
	GameObject(std::string identifier, StringMapMap& setupMap, std::function<GameObject*(const std::string& identifier)> idGoMapper);
	virtual ~GameObject();

	void applySetupMap(StringMapMap& setupMap);

	void applyConfig(Config config);
	Config getConfig();

	virtual void update(sf::Time ellapsed) override;
	virtual void event(Event e);

	virtual void onObjectCollision(ph::Manifold* manifold, GameObject* go);
	virtual void onTileCollision(ph::Manifold* manifold, Tile* t);

	void onCollision(ph::Manifold* manifold) override;
	const std::string getCallbackType() const override;

	const std::string getType() const;

	static GameObject* castBodyCallback(ph::Body::Callback* c);

	void toggleLogging();

	ph::Body* getBody() const;

	const std::string getIdentifier() const;

	std::string getName();
	void setName(std::string s);

	sf::Color getColor() const;
	void setColor(sf::Color s);

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

	int getZindex() const;
	void setZindex(int s);

	bool isMoving() const;
	bool isFalling() const;
	bool isRising() const;
	// changes moving, falling, rising flags
	void updateFlags();

	void apply();

	const Extension& getExtension(const std::string& name) const;

	// Write all the values that should be updated every tick into this function
	void updateLog() const;
private:
	ph::Shape* createPolyShape(std::vector<ph::Vec2>& vertices, float density = 1);
	ph::Shape* createCircleShape(float radius, float density = 1);
	void initBody(ph::Body::Config config, ph::Shape* shape);
	void initSetupMap(StringMapMap& setupMap);
	void initExtension(std::string extensionName, StringMapMap& setupMap);

	bool m_log = false;
	bool m_visible = true;
	const std::string m_type;
	const std::string m_identifier;
	std::string m_name;
	sf::Color m_color = sf::Color::White;
	int m_zindex = 1;

	ph::Body* m_body = NULL;

	bool m_isRising = false; // movement to top
	bool m_isFalling = false; // movement to bottom
	bool m_isMoving = false;  // movement on x

	std::function<GameObject*(const std::string& identifier)> m_idGoMapper;
	std::map<std::string, Extension*> m_extensions;
};

#endif

