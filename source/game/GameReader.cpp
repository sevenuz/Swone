#include "game/GameReader.h"
#include "game/Map.h"
#include "util/reader/MapReader.h"

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

void GameReader::hashResDir(std::string resDir)
{
	auto fn = [&](tinydir_file& file){
		try {
			std::string path(file.path);
			std::string hash = md5file(file.path);
			Log::ger().log(path);
			if(getFileHashes().count(hash) && getFileHashes()[hash] != path)
				Log::ger().log("Collision: " + hash, Log::Label::Error);
			getFileHashes()[hash] = path;
		} catch(const std::invalid_argument& ia) {
			Log::ger().log(ia.what(), Log::Label::Error);
		}
	};
	Helper::readDirectory(GameReader::getSceneryPath(resDir), fn, true);
	Helper::readDirectory(GameReader::getTexturePath(resDir), fn, true);
	Helper::readDirectory(GameReader::getMapPath(resDir), fn, true);
	Helper::readDirectory(GameReader::getGameObjectPath(resDir), fn, true);
}

const sf::Texture* GameReader::loadTexture(std::string path)
{
	if(getTextureMap().count(path)){
		return getTextureMap()[path];
	} else {
		sf::Texture* t = new sf::Texture();
		if (!t->loadFromFile(path)) {
			Log::ger().log(path + ": Failed to load texture", Log::Label::Error);
			throw std::invalid_argument("Failed to load texture");
		}
		getTextureMap()[path] = t;
		return t;
	}
}

const sf::Image* GameReader::loadImage(std::string path)
{
	if(getImageMap().count(path)){
		return getImageMap()[path];
	} else {
		sf::Image* t = new sf::Image();
		if (!t->loadFromFile(path)) {
			Log::ger().log(path + ": Failed to load texture", Log::Label::Error);
			throw std::invalid_argument("Failed to load texture");
		}
		getImageMap()[path] = t;
		return t;
	}
}
