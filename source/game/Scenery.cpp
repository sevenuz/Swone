#include <game/Scenery.h>

Scenery::Scenery(std::string path, StringMapMap setupMap, GameReader& gameReader) : m_setupMap(setupMap) {
	m_fileMap[path] = md5file(path.c_str());
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH)){
		auto& global = setupMap[Reader::DEFAULT_PARAGRAPH];
		if(global.count(S_NAME))
			m_name = global[S_NAME];
		else
			throw std::invalid_argument("Name missing.");
		if(global.count(S_MAP)) {
			std::string mapPath = gameReader.getMapPath(global[S_MAP]);
			m_fileMap[mapPath] = md5file(mapPath.c_str());

			m_map = gameReader.getMap(mapPath);

			std::string texturePath = m_map->getTileTexturePath();
			m_fileMap[texturePath] = md5file(texturePath.c_str());
		} else
			throw std::invalid_argument("Map-Filename missing.");
	}

	if(setupMap.count(Scenery::S_PLAYER_PARAGRAPH)){
		for(int i = 1; setupMap[Scenery::S_PLAYER_PARAGRAPH].count(std::to_string(i)); i++){
			std::string pName = setupMap[Scenery::S_PLAYER_PARAGRAPH][std::to_string(i)];
			m_playerSetupMaps[pName] = getGameObjectSetupMap(gameReader, pName);
		}
	}

	for(auto& m : setupMap) {
		std::string paragraph = m.first;
		if(paragraph == Reader::DEFAULT_PARAGRAPH)
			continue;
		if(paragraph == Scenery::S_PLAYER_PARAGRAPH)
			continue;

		m_objectSetupMaps[paragraph] = getGameObjectSetupMap(gameReader, paragraph);
	}
}

Scenery::~Scenery() {
	for(GameObject* go : m_gameObjects)
		delete go;
	m_gameObjects.clear();
}

StringMapMap& Scenery::getGameObjectSetupMap(GameReader& gameReader, std::string goName)
{
	std::string goPath = gameReader.getGameObjectPath(goName);
	m_fileMap[goPath] = md5file(goPath.c_str());

	StringMapMap& goSetupMap = gameReader.getGameObjectParagraphMap(goPath);

	std::string texturePath = goSetupMap[Reader::DEFAULT_PARAGRAPH][GameObject::S_TEXTURE];
	m_fileMap[texturePath] = md5file(texturePath.c_str());

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
	getMap()->event(event);

	for (GameObject* g : getGameObjects()) {
		g->event(event);
	}
}

void Scenery::update(sf::Time ellapsed)
{
	getMap()->update(ellapsed);

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
		if(g->getZindex() > 0)
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

const StringMap& Scenery::getFileList() const
{
	return m_fileMap;
}

void Scenery::setName(std::string n)
{
	m_name = n;
}

std::string Scenery::getName()
{
	return m_name;
}
