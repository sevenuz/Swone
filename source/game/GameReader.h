#ifndef SWONE_GAME_GAMEREADER_H
#define SWONE_GAME_GAMEREADER_H

#include <functional>
#include <map>

#include <md5.h>

#include "util/Log.h"
#include "util/reader/Reader.h"

class Map;

namespace GameReader {
	static constexpr const char* GFC_FILE_EXTENSION = "Gfc";

	static constexpr const char* RES_DIR_MAP = "map/";
	static constexpr const char* RES_DIR_OBJECT = "obj/";
	static constexpr const char* RES_DIR_SCENERY = "scenery/";
	static constexpr const char* RES_DIR_TEXTURE = "texture/";

	StringMapMap& getSceneryMap(std::string file);
	Map* getMap(std::string file, std::function<std::string(std::string textureName)> textureMapper);
	StringMapMap& getGameObjectMap(std::string file);

	std::string getTexturePath(std::string resDir, std::string name = "");
	std::string getSceneryPath(std::string resDir, std::string name = "");
	std::string getMapPath(std::string resDir, std::string name = "");
	std::string getGameObjectPath(std::string resDir, std::string name = "");

	void readSceneryMaps(std::string resDir);
	void hashResDir(std::string resDir);

	std::string getHash(std::string file);
	std::string getFile(std::string hash);

	const sf::Texture* loadTexture(std::string path);
	const sf::Image* loadImage(std::string path);

	// is only once created and will be destroyed on termination
	inline StringMap& getFileHashes() {
		static auto* map = new StringMap; // first: checksum, second: filePath
		return *map;
	};
	inline std::map<std::string, StringMapMap>& getSceneryMaps() {
		static auto* map = new std::map<std::string, StringMapMap>;
		return *map;
	};
	inline std::map<std::string, Map*>& getMapMap() {
		static auto* map = new std::map<std::string, Map*>;
		return *map;
	};
	inline std::map<std::string, StringMapMap>& getGameObjectMaps() {
		static auto* map = new std::map<std::string, StringMapMap>;
		return *map;
	};
	inline std::map<std::string, sf::Texture*>& getTextureMap() {
		static auto* map = new std::map<std::string, sf::Texture*>;
		return *map;
	};
	inline std::map<std::string, sf::Image*>& getImageMap() {
		static auto* map = new std::map<std::string, sf::Image*>;
		return *map;
	};
};
#endif

