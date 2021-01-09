#ifndef SWONE_GAME_GAMEREADER_H
#define SWONE_GAME_GAMEREADER_H

#include <map>

#include "game/Map.h"
#include "util/Log.h"
#include "util/reader/MapReader.h"

#define RES_DIR_MAP "map/"
#define RES_DIR_OBJECT "obj/"
#define RES_DIR_SCENERY "scenery/"
#define RES_DIR_TEXTURE "texture/"

class GameReader {
public:
	// Not posible to create Objects of this class
	GameReader() = delete;
	~GameReader() {};
	GameReader(GameReader const&) = delete;
	void operator=(GameReader const&) = delete;

	static StringMapMap& getSceneryMap(std::string file);
	static Map* getMap(std::string resDir, std::string file);
	static StringMapMap& getGameObjectMap(std::string file);

	static std::string getTexturePath(std::string resDir, std::string name = "");
	static std::string getSceneryPath(std::string resDir, std::string name = "");
	static std::string getMapPath(std::string resDir, std::string name = "");
	static std::string getGameObjectPath(std::string resDir, std::string name = "");

	static void readSceneryMaps(std::string resDir);

	// is only once created and will be destroyed on termination
	static std::map<std::string, StringMapMap>& getSceneryMaps() {
		static auto* map = new std::map<std::string, StringMapMap>;
		return *map;
	};
	static std::map<std::string, Map*>& getMapMap() {
		static auto* map = new std::map<std::string, Map*>;
		return *map;
	};
	static std::map<std::string, StringMapMap>& getGameObjectMaps() {
		static auto* map = new std::map<std::string, StringMapMap>;
		return *map;
	};
};
#endif

