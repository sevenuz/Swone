#ifndef SWONE_GAME_SCENERY_H
#define SWONE_GAME_SCENERY_H

#include <list>
#include <map>

#include <md5.h>

#include "Handleable.h"
#include "game/GameReader.h"
#include "game/Map.h"
#include "game/object/GameObject.h"

class Scenery : public Handleable {
public:
	static constexpr const char* S_NAME = "name";
	static constexpr const char* S_MAP = "map";

	static constexpr const char* S_PLAYER_PARAGRAPH = "player";

	Scenery(std::string resDir, std::string fileName, StringMapMap setupMap);
	virtual ~Scenery();

	void drawPreview(sf::RenderTarget& target, sf::RenderStates states) const;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void event(sf::Event& e);
	void update(sf::Time ellapsed);

	Map* getMap();
	const Map* getMap() const;
	std::list<GameObject*>& getGameObjects();
	const std::list<GameObject*>& getGameObjects() const;

	std::map<std::string, StringMapMap>& getPlayerSetupMaps();
	std::map<std::string, StringMapMap>& getObjectSetupMaps();
	const StringPair& getSceneryFile() const;
	const StringPair& getMapFile() const;
	const StringMap& getTextureFileMap() const;
	const StringMap& getObjectFileMap() const;

	void setName(std::string);
	std::string getName();

	void spawnPlayer(std::string key);
	void reset();
private:
	StringMapMap& getGameObjectSetupMap(std::string resDir, std::string goName);
	void sortGameObjects();
	void spawnGameObjects();

	std::string m_name = "";

	Map* m_map = NULL;
	std::list<GameObject*> m_gameObjects;

	StringMapMap m_setupMap;
	std::map<std::string, StringMapMap> m_playerSetupMaps;
	std::map<std::string, StringMapMap> m_objectSetupMaps;

	StringPair m_sceneryFile;
	StringPair m_mapFile;
	StringMap m_textureFileMap;
	StringMap m_objectFileMap;
};
#endif

