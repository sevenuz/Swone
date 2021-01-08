#include "game/GameReader.h"
#include "game/Scenery.h"

GameReader::GameReader(std::string resDir) :
	m_resDir(resDir),
	m_mapReader(getTextureBasePath())
{
	readSceneries();
}

GameReader::~GameReader()
{
	for(Scenery* s : m_sceneries)
		delete s;
	for(auto& m : m_maps)
		delete m.second;
}

const std::vector<Scenery*>& GameReader::getSceneries() const
{
	return m_sceneries;
}

void GameReader::readSceneries()
{
	std::string dir = m_resDir + RES_DIR_SCENERY;
	Helper::readDirectory(dir,
		[&](tinydir_file& file){
			try {
				if (!file.is_dir)
				{
					std::string sceneryPath(file.path);
					std::string fileName(file.name);
					// TODO should only once create Reader
					Reader r(sceneryPath);
					Scenery* s = new Scenery(sceneryPath, fileName, r.copyParagraphMap(), *this);
					m_sceneries.push_back(s);
				}
			} catch(const std::invalid_argument& ia) {
				Log::ger().log(ia.what(), Log::Label::Error);
			}
		}
	);
}

std::string GameReader::getTextureBasePath()
{
	return m_resDir + RES_DIR_TEXTURE;
}

std::string GameReader::getSceneryBasePath() {
 return m_resDir + RES_DIR_SCENERY;
}

std::string GameReader::getMapBasePath() {
 return m_resDir + RES_DIR_MAP;
}

std::string GameReader::getObjectBasePath() {
 return m_resDir + RES_DIR_OBJECT;
}

Map* GameReader::getMap(std::string file)
{
	if(!m_maps.count(file)) {
		Map* map = new Map();

		m_mapReader.setPath(file);

		m_mapReader.setMap(map);
		m_mapReader.read();

		m_maps[file] = map;
	}
	return m_maps[file];
}

StringMapMap& GameReader::getGameObjectParagraphMap(std::string file)
{
	if(!m_objects.count(file)) {
		Reader r(file);
		m_objects[file] = r.copyParagraphMap();
	}
	return m_objects[file];
}
