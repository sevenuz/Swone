#ifndef SWONE_GAME_SCENERY_H
#define SWONE_GAME_SCENERY_H

#include <list>
#include <map>

#include "game/GameReader.h"
#include "game/Map.h"
#include "game/Net.h"
#include "game/object/Extension.h"
#include "game/object/GameObject.h"

class Scenery {
public:
	static constexpr const char* S_NAME = "name";
	static constexpr const char* S_MAP = "map";

	static constexpr const char* S_PLAYER_PARAGRAPH = "player";

	Scenery();
	Scenery(std::string resDir, std::string fileName, StringMapMap setupMap);
	virtual ~Scenery();

	Map* getMap();
	const Map* getMap() const;

	const std::string& getName() const;

	StringMapMap& getSetupMap();
	std::map<std::string, StringMapMap>& getPlayerSetupMaps();
	std::map<std::string, StringMapMap>& getObjectSetupMaps();
	const Net::GameFileCheck& getFileCheck() const;

	std::vector<std::string>& getBeginningGameObjects();
	std::vector<std::string>& getStaticGameObjects();
private:
	StringMapMap& getGameObjectSetupMap(std::string resDir, std::string goName, bool gfcInjection);

	std::string m_name = "";

	Map* m_map = nullptr;

	StringMapMap m_setupMap;
	std::map<std::string, StringMapMap> m_playerSetupMaps;
	std::map<std::string, StringMapMap> m_objectSetupMaps;
	Net::GameFileCheck m_fileCheck;

	std::vector<std::string> m_beginningGameObjects; // keys of objects, which are spawned at the start
	// keys of objects, which are solid and uncollisionableUnsolid, will be spawned automaticly at start
	std::vector<std::string> m_staticGameObjects;
};
#endif

