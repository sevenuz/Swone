#include "game/GameReader.h"
#include "game/Map.h"
#include "util/reader/MapReader.h"

void GameReader::initResDir(const std::string& s)
{
	std::filesystem::create_directories(s);
	std::filesystem::create_directories(GameReader::getSceneryPath(s));
	std::filesystem::create_directories(GameReader::getMapPath(s));
	std::filesystem::create_directories(GameReader::getGameObjectPath(s));
	std::filesystem::create_directories(GameReader::getTexturePath(s));
}

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
		auto sm = r.copyParagraphMap();
		try {
			Reader rgfc(file + GFC_FILE_EXTENSION);
			sm.insert(rgfc.getParagraphMap().begin(), rgfc.getParagraphMap().end());
		}
		catch (const std::invalid_argument& ia) {} // normal with original game files
		getSceneryMaps()[file] = sm;
	}
	return getSceneryMaps()[file];
}

Map* GameReader::getMap(std::string file, std::function<std::string(std::string textureName)> fn)
{
	if(!getMapMap().count(file)) {
		Map* map = new Map();

		MapReader mapReader(fn);
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
			getHash(std::string(file.path));
		} catch(const std::invalid_argument& ia) {
			Log::ger().log(ia.what(), Log::Label::Error);
		}
	};
	Helper::readDirectory(GameReader::getSceneryPath(resDir), fn, true);
	Helper::readDirectory(GameReader::getTexturePath(resDir), fn, true);
	Helper::readDirectory(GameReader::getMapPath(resDir), fn, true);
	Helper::readDirectory(GameReader::getGameObjectPath(resDir), fn, true);
}

std::string GameReader::getHash(std::string file)
{
	std::string hash = Helper::md5(file.c_str());
	// TODO is hash collision a problem?
	if(getFileHashes().count(hash) && getFileHashes()[hash] != file) {
		Log::ger().log("Collision: " + hash, Log::Label::Error);
	}
	getFileHashes()[hash] = file;
	return hash;
}

std::string GameReader::getFile(std::string hash)
{
	// TODO different exception handling on file not found?
	if(!getFileHashes().count(hash)) {
		Log::ger().log("File(" + hash + ") not indexed or not in resDir", Log::Label::Error);
		throw std::invalid_argument("hash not found");
	}
	return getFileHashes()[hash];
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
