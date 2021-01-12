#ifndef SWONE_GAME_GAMEREADER_H
#define SWONE_GAME_GAMEREADER_H

#include <functional>
#include <map>

#include <md5.h>

#include "util/Log.h"
#include "util/reader/Reader.h"

class Map;

class GameReader {
public:
	// Not posible to create Objects of this class
	GameReader() = delete;
	~GameReader() {};
	GameReader(GameReader const&) = delete;
	void operator=(GameReader const&) = delete;

	static constexpr const char* GFC_FILE_EXTENSION = "Gfc";

	static constexpr const char* RES_DIR_MAP = "map/";
	static constexpr const char* RES_DIR_OBJECT = "obj/";
	static constexpr const char* RES_DIR_SCENERY = "scenery/";
	static constexpr const char* RES_DIR_TEXTURE = "texture/";

	static StringMapMap& getSceneryMap(std::string file);
	static Map* getMap(std::string file, std::function<std::string(std::string textureName)> textureMapper);
	static StringMapMap& getGameObjectMap(std::string file);

	static std::string getTexturePath(std::string resDir, std::string name = "");
	static std::string getSceneryPath(std::string resDir, std::string name = "");
	static std::string getMapPath(std::string resDir, std::string name = "");
	static std::string getGameObjectPath(std::string resDir, std::string name = "");

	static void readSceneryMaps(std::string resDir);
	static void hashResDir(std::string resDir);

	static std::string getHash(std::string file);
	static std::string getFile(std::string hash);

	static const sf::Texture* loadTexture(std::string path);
	static const sf::Image* loadImage(std::string path);

	// is only once created and will be destroyed on termination
	static StringMap& getFileHashes() {
		static auto* map = new StringMap; // first: checksum, second: filePath
		return *map;
	};
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
	static std::map<std::string, sf::Texture*>& getTextureMap() {
		static auto* map = new std::map<std::string, sf::Texture*>;
		return *map;
	};
	static std::map<std::string, sf::Image*>& getImageMap() {
		static auto* map = new std::map<std::string, sf::Image*>;
		return *map;
	};
};
#endif

