#include <game/Scenery.h>

Scenery::Scenery(std::string resDir, std::string fileName, StringMapMap setupMap) :
	m_setupMap(setupMap)
{
	m_sceneryFile = std::make_pair(fileName, md5file(GameReader::getSceneryPath(resDir, fileName).c_str()));
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH)) {
		auto& global = setupMap[Reader::DEFAULT_PARAGRAPH];
		if(global.count(S_NAME))
			m_name = global[S_NAME];
		else
			throw std::invalid_argument("Name missing.");
		if(global.count(S_MAP)) {
			std::string mapName = global[S_MAP];
			std::string mapPath = GameReader::getMapPath(resDir, mapName);
			m_mapFile = std::make_pair(mapName, md5file(mapPath.c_str()));

			m_map = GameReader::getMap(resDir, mapPath);

			std::string textureName = m_map->getTileTextureName();
			std::string texturePath = m_map->getTileTexturePath();
			m_textureFileMap[textureName] = md5file(texturePath.c_str());
		} else {
			throw std::invalid_argument("Map-Filename missing.");
		}
	}

	if(setupMap.count(Scenery::S_PLAYER_PARAGRAPH)) {
		for(int i = 1; setupMap[Scenery::S_PLAYER_PARAGRAPH].count(std::to_string(i)); i++) {
			std::string pName = setupMap[Scenery::S_PLAYER_PARAGRAPH][std::to_string(i)];
			m_playerSetupMaps[pName] = getGameObjectSetupMap(resDir, pName);
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

		m_objectSetupMaps[paragraph] = getGameObjectSetupMap(resDir, paragraph);
	}
}

Scenery::~Scenery() {
	for(GameObject* go : m_gameObjects)
		delete go;
	m_gameObjects.clear();
}

StringMapMap& Scenery::getGameObjectSetupMap(std::string resDir, std::string goName)
{
	std::string goPath = GameReader::getGameObjectPath(resDir, goName);
	m_objectFileMap[goName] = md5file(goPath.c_str());

	StringMapMap& goSetupMap = GameReader::getGameObjectMap(goPath);

	std::string textureName = goSetupMap[Reader::DEFAULT_PARAGRAPH][GameObject::S_TEXTURE];
	std::string texturePath = GameReader::getTexturePath(resDir, textureName);

	// set texture path in setupMap
	goSetupMap[Reader::DEFAULT_PARAGRAPH][GameObject::S_TEXTURE_PATH] = texturePath;
	m_textureFileMap[textureName] = md5file(texturePath.c_str());

	return goSetupMap;
}

void Scenery::sortGameObjects()
{
	// sort list with z-index
	m_gameObjects.sort([](GameObject* a, GameObject* b) -> bool {
		return a->getZindex() < b->getZindex();
	});
}

void Scenery::spawnPlayer(std::string key)
{
	GameObject* go = new GameObject(m_playerSetupMaps[key]);
	m_gameObjects.push_back(go);
}

void Scenery::spawnGameObjects()
{
	for(auto& osm : m_objectSetupMaps) {
		std::string paragraph = osm.first;
		StringMapMap& goSetupMap = osm.second;
		for(int i = 1; m_setupMap[paragraph].count(std::to_string(i)); i++){
			GameObject* go = new GameObject(goSetupMap);
			go->setPos(Helper::toVector2f(m_setupMap[paragraph][std::to_string(i)]));
			m_gameObjects.push_back(go);
		}
	}
}

void Scenery::reset()
{
	for(GameObject* go : m_gameObjects)
		delete go;
	m_gameObjects.clear();
	spawnGameObjects();
	sortGameObjects();
}

void Scenery::event(sf::Event& event)
{
	for (GameObject* g : getGameObjects()) {
		g->event(event);
	}
}

void Scenery::update(sf::Time ellapsed)
{
	for (GameObject* g : getGameObjects()) {
		g->apply();
		g->update(ellapsed);
	}

	sortGameObjects();
}

void Scenery::drawPreview(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;

	target.draw(*getMap(), states);
	for (GameObject* g : getGameObjects())
		if(g->getType() != GameObject::S_PLAYER_TYPE && g->getZindex() > 0)
			target.draw(*g, states);
};

void Scenery::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;

	bool drawMap = true;
	for (GameObject* g : getGameObjects()) {
		if(g->getZindex() >= 0 && drawMap) {
			target.draw(*getMap(), states);
			drawMap = false;
		}
		target.draw(*g, states);
	}
	if(drawMap)
		target.draw(*getMap(), states);
};

Map* Scenery::getMap()
{
	return m_map;
}

const Map* Scenery::getMap() const
{
	return m_map;
}

std::list<GameObject*>& Scenery::getGameObjects()
{
	return m_gameObjects;
}

const std::list<GameObject*>& Scenery::getGameObjects() const
{
	return m_gameObjects;
}

std::map<std::string, StringMapMap>& Scenery::getPlayerSetupMaps()
{
	return m_playerSetupMaps;
}

std::map<std::string, StringMapMap>& Scenery::getObjectSetupMaps()
{
	return m_objectSetupMaps;
}

const StringPair& Scenery::getSceneryFile() const
{
	return m_sceneryFile;
}

const StringPair& Scenery::getMapFile() const
{
	return m_mapFile;
}

const StringMap& Scenery::getTextureFileMap() const
{
	return m_textureFileMap;
}

const StringMap& Scenery::getObjectFileMap() const
{
	return m_objectFileMap;
}

void Scenery::setName(std::string n)
{
	m_name = n;
}

std::string Scenery::getName()
{
	return m_name;
}
