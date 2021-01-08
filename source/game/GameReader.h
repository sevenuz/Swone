#ifndef SWONE_GAME_GAMEREADER_H
#define SWONE_GAME_GAMEREADER_H

#include <vector>
#include <map>

#include "game/Map.h"
#include "game/object/GameObject.h"
#include "util/Log.h"
#include "util/reader/MapReader.h"

#define RES_DIR_MAP "map/"
#define RES_DIR_OBJECT "obj/"
#define RES_DIR_SCENERY "scenery/"
#define RES_DIR_TEXTURE "texture/"

class Scenery;

class GameReader {
public:
	GameReader(std::string resDir);
	virtual ~GameReader();

	void readSceneries();
	Map* getMap(std::string mapFileName);
	StringMapMap& getGameObjectParagraphMap(std::string objFileName);

	std::string getTextureBasePath();
	std::string getSceneryBasePath();
	std::string getMapBasePath();
	std::string getObjectBasePath();

	const std::vector<Scenery*>& getSceneries() const;
private:
	std::string m_resDir;
	MapReader m_mapReader;

	std::vector<Scenery*> m_sceneries;
	std::map<std::string, Map*> m_maps;
	std::map<std::string, StringMapMap> m_objects;
};
#endif

