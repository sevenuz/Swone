#ifndef SWONE_GAME_GAMEREADER_H
#define SWONE_GAME_GAMEREADER_H

#include <vector>

#include "game/Map.h"
#include "game/object/GameObject.h"
#include "util/Log.h"
#include "util/reader/MapReader.h"

#define RES_DIR_MAP "map/"
#define RES_DIR_OBJECT "obj/"
#define RES_DIR_SCENERY "scn/"

class GameReader {
public:
	GameReader();
	virtual ~GameReader();

	void read(std::string resDir);
	//void readSceneriesFromDir(std::string resDir);
	void readMapsFromDir(std::string resDir);
	void readGameObjectsFromDir(std::string resDir);

	//const std::vector<Scenery*>& getSceneries() const;
	const std::vector<Map*>& getMaps() const;
	const std::vector<GameObject*>& getGameObjects() const;
private:
	MapReader m_mapReader;

	//std::vector<Scenery*> m_sceneries;
	std::vector<Map*> m_maps;
	std::vector<GameObject*> m_objects;
};
#endif

