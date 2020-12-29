#include "game/GameReader.h"

GameReader::GameReader() {}
GameReader::~GameReader() {}

//std::vector<Scenery*>& GameReader::getSceneries() {
//	return m_sceneries;
//}

const std::vector<Map*>& GameReader::getMaps() const {
	return m_maps;
}

const std::vector<GameObject*>& GameReader::getGameObjects() const {
	return m_objects;
}

void GameReader::read(std::string resDir) {
	//readSceneryFromDir(resDir);
	readMapsFromDir(resDir);
	readGameObjectsFromDir(resDir);
}
			/*
void GameReader::readSceneryFromDir(std::string resDir) {
	std::string dir = resDir + RES_DIR_SCENERY;
	Helper::readDirectory(dir,
		[&](tinydir_file& file){
			try {
				if (!file.is_dir)
				{
					std::stringstream ss;
					ss << dir << file.name;
					// TODO should only once create Reader
					Reader r(ss.str());
					Scenery* s = new Scenery(r.getParagraphMap());
					m_objects.push_back(s);
				}
			} catch(const std::invalid_argument& ia) {
				Log::ger().log(ia.what(), Log::Label::Error);
			}
		}
	);
}
	*/
void GameReader::readMapsFromDir(std::string resDir) {
	std::string dir = resDir + RES_DIR_MAP;
	Helper::readDirectory(dir,
		[&](tinydir_file& file){
			try {
				if (!file.is_dir)
				{
					Map* map = new Map();

					std::stringstream ss;
					ss << dir << file.name;
					m_mapReader.setPath(ss.str());

					m_mapReader.setMap(map);
					m_mapReader.read();

					m_maps.push_back(map);
				}
			} catch(const std::invalid_argument& ia) {
				Log::ger().log(ia.what(), Log::Label::Error);
			}
		}
	);
}

void GameReader::readGameObjectsFromDir(std::string resDir) {
	std::string dir = resDir + RES_DIR_OBJECT;
	Helper::readDirectory(dir,
		[&](tinydir_file& file){
			try {
				if (!file.is_dir)
				{
					std::stringstream ss;
					ss << dir << file.name;
					// TODO should only once create Reader
					Reader r(ss.str());
					GameObject* o = new GameObject(r.getParagraphMap());
					m_objects.push_back(o);
				}
			} catch(const std::invalid_argument& ia) {
				Log::ger().log(ia.what(), Log::Label::Error);
			}
		}
	);
}
