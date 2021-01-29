#include "game/Scenery.h"

Scenery::Scenery() {}

Scenery::Scenery(std::string resDir, std::string fileName, StringMapMap setupMap) :
	m_setupMap(setupMap)
{
	bool gfcInjection = setupMap.count(Net::GFC_SCENERY_FILE);

	if(gfcInjection) {
		for(auto p : setupMap[Net::GFC_SCENERY_FILE])
			m_fileCheck.sceneryFile = p;
		for(auto p : setupMap[Net::GFC_MAP_FILE])
			m_fileCheck.mapFile = p;
		m_fileCheck.textureFileMap = setupMap[Net::GFC_TEXTURE_FILE_MAP];
		m_fileCheck.objectFileMap = setupMap[Net::GFC_OBJECT_FILE_MAP];
	} else {
		m_fileCheck.sceneryFile = std::make_pair(
				fileName,
				md5file(GameReader::getSceneryPath(resDir, fileName).c_str())
		);
	}
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH)) {
		auto& global = setupMap[Reader::DEFAULT_PARAGRAPH];
		if(global.count(S_NAME))
			m_name = global[S_NAME];
		else
			throw std::invalid_argument("Name missing.");
		if(global.count(S_MAP)) {
			std::string mapPath;
			if(gfcInjection) {
				mapPath = GameReader::getFile(m_fileCheck.mapFile.second);
			} else {
				std::string mapName = global[S_MAP];
				mapPath = GameReader::getMapPath(resDir, mapName);
				m_fileCheck.mapFile = std::make_pair(mapName, md5file(mapPath.c_str()));
			}

			m_map = GameReader::getMap(mapPath, [&](std::string textureName) -> std::string {
				if(gfcInjection) {
					return GameReader::getFile(m_fileCheck.textureFileMap[textureName]);
				} else {
					return GameReader::getTexturePath(resDir, textureName);
				}
			});

			if(!gfcInjection) {
				std::string textureName = m_map->getTileTextureName();
				std::string texturePath = m_map->getTileTexturePath();
				m_fileCheck.textureFileMap[textureName] = md5file(texturePath.c_str());
			}
		} else {
			throw std::invalid_argument("Map-Filename missing.");
		}
	}

	if(setupMap.count(Scenery::S_PLAYER_PARAGRAPH)) {
		for(int i = 1; setupMap[Scenery::S_PLAYER_PARAGRAPH].count(std::to_string(i)); i++) {
			std::string pName = setupMap[Scenery::S_PLAYER_PARAGRAPH][std::to_string(i)];
			m_playerSetupMaps[pName] = getGameObjectSetupMap(resDir, pName, gfcInjection);
			if(m_playerSetupMaps[pName][Reader::DEFAULT_PARAGRAPH][GameObject::S_TYPE] != GameObject::S_PLAYER_TYPE)
				throw std::invalid_argument("All Files in Player-Paragraph have to be from type player.");
		}
	} else {
		throw std::invalid_argument("Player-Paragraph missing.");
	}

	for(auto& m : setupMap) {
		std::string paragraph = m.first;
		if(paragraph == Reader::DEFAULT_PARAGRAPH)
			continue;
		if(paragraph == Scenery::S_PLAYER_PARAGRAPH)
			continue;
		if(paragraph == Net::GFC_SCENERY_FILE)
			continue;
		if(paragraph == Net::GFC_MAP_FILE)
			continue;
		if(paragraph == Net::GFC_TEXTURE_FILE_MAP)
			continue;
		if(paragraph == Net::GFC_OBJECT_FILE_MAP)
			continue;

		m_objectSetupMaps[paragraph] = getGameObjectSetupMap(resDir, paragraph, gfcInjection);
		// TODO spawn system!!!
		if(m_objectSetupMaps[paragraph][Reader::DEFAULT_PARAGRAPH][GameObject::S_TYPE] == GameObject::S_STATIC_TYPE)
			m_staticGameObjects.push_back(paragraph);
		else
			m_beginningGameObjects.push_back(paragraph);
	}
}

Scenery::~Scenery()
{}

StringMapMap& Scenery::getGameObjectSetupMap(std::string resDir, std::string goName, bool gfcInjection)
{
	std::string goPath;
	if(gfcInjection) {
		goPath = GameReader::getFile(m_fileCheck.objectFileMap[goName]);
	} else {
		goPath = GameReader::getGameObjectPath(resDir, goName);
		m_fileCheck.objectFileMap[goName] = md5file(goPath.c_str());
	}

	StringMapMap& goSetupMap = GameReader::getGameObjectMap(goPath);

	std::string textureName = goSetupMap[Reader::DEFAULT_PARAGRAPH][GameObject::S_TEXTURE];
	std::string texturePath;
	if(gfcInjection) {
		texturePath = GameReader::getFile(m_fileCheck.textureFileMap[textureName]);
	} else {
		texturePath = GameReader::getTexturePath(resDir, textureName);
	}

	// prevent controls from gameobject files
	if(goSetupMap.count(Extension::CONTROLS_PARAGRAPH)){
		goSetupMap[Extension::CONTROLS_PARAGRAPH].clear();
	}

	// set texture path in setupMap
	goSetupMap[Reader::DEFAULT_PARAGRAPH][GameObject::S_TEXTURE_PATH] = texturePath;
	if(!gfcInjection)
		m_fileCheck.textureFileMap[textureName] = md5file(texturePath.c_str());

	return goSetupMap;
}

Map* Scenery::getMap()
{
	return m_map;
}

const Map* Scenery::getMap() const
{
	return m_map;
}

StringMapMap& Scenery::getSetupMap()
{
	return m_setupMap;
}

std::map<std::string, StringMapMap>& Scenery::getPlayerSetupMaps()
{
	return m_playerSetupMaps;
}

std::map<std::string, StringMapMap>& Scenery::getObjectSetupMaps()
{
	return m_objectSetupMaps;
}

const Net::GameFileCheck& Scenery::getFileCheck() const
{
	return m_fileCheck;
}

std::vector<std::string>& Scenery::getBeginningGameObjects()
{
	return m_beginningGameObjects;
}

std::vector<std::string>& Scenery::getStaticGameObjects()
{
	return m_staticGameObjects;
}

const std::string& Scenery::getName() const
{
	return m_name;
}
