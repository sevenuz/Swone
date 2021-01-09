#include "game/GameReader.h"

std::string GameReader::getTexturePath(std::string resDir, std::string name)
{
	return resDir + RES_DIR_TEXTURE + name;
}

std::string GameReader::getSceneryPath(std::string resDir, std::string name) {
 return resDir + RES_DIR_SCENERY + name;
}

std::string GameReader::getMapPath(std::string resDir, std::string name) {
 return resDir + RES_DIR_MAP + name;
}

std::string GameReader::getGameObjectPath(std::string resDir, std::string name) {
 return resDir + RES_DIR_OBJECT + name;
}

StringMapMap& GameReader::getSceneryMap(std::string file)
{
	if(!getSceneryMaps().count(file)) {
		Reader r(file);
		getSceneryMaps()[file] = r.copyParagraphMap();
	}
	return getSceneryMaps()[file];
}

Map* GameReader::getMap(std::string resDir, std::string file)
{
	if(!getMapMap().count(file)) {
		Map* map = new Map();

		static MapReader mapReader(getTexturePath(resDir));
		mapReader.setPath(file);

		mapReader.setMap(map);
		mapReader.read();

		getMapMap()[file] = map;
	}
	return getMapMap()[file];
}

StringMapMap& GameReader::getGameObjectMap(std::string file)
{
	if(!getGameObjectMaps().count(file)) {
		Reader r(file);
		getGameObjectMaps()[file] = r.copyParagraphMap();
	}
	return getGameObjectMaps()[file];
}

void GameReader::readSceneryMaps(std::string resDir)
{
	std::string dir = GameReader::getSceneryPath(resDir);
	Helper::readDirectory(dir,
		[&](tinydir_file& file){
			try {
				if (!file.is_dir)
				{
					std::string sceneryPath(file.path);
					getSceneryMap(sceneryPath);
				}
			} catch(const std::invalid_argument& ia) {
				Log::ger().log(ia.what(), Log::Label::Error);
			}
		}
	);
}
