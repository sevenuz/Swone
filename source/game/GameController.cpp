#include <game/GameController.h>

GameController::GameController() {}

GameController::~GameController()
{
	clearAll();
}

void GameController::loadScenery(std::string resDir, Net::GameFileCheck gfc) {
	clearAll();

	std::string path = GameReader::getFile(gfc.sceneryFile.second);
	m_scenery = Scenery(resDir, Helper::parseFileName(path), GameReader::getSceneryMap(path));

	// add map tiles to scene
	for(auto& mm : getMap()->getMapData())
		for(auto& mmm : mm.second)
			if(!mmm.second->isPassable())
				m_scene.Add(mmm.second->body);

	spawnStaticGameObjects();
}

void GameController::sortAll()
{
	// sort list with z-index
	m_all.sort([](GameObject* a, GameObject* b) -> bool {
		return a->getZindex() < b->getZindex();
	});
}

void GameController::removeFromGame(GameObject* go)
{
	m_players.remove(go);
	m_staticGameObjects.remove(go);
	m_gameObjects.remove(go);
	m_all.remove(go);

	m_scene.Remove(go->getBody());
	m_goPointers.erase(go->getIdentifier());
	m_goKeys.erase(go->getIdentifier());
	delete go;
}

void GameController::clearAll()
{
	// dont use removeFromGame to make subclassing possible // TODO true?
	for(GameObject* go : m_all) {
		delete go;
	}
	m_scene.Clear();
	m_players.clear();
	m_staticGameObjects.clear();
	m_gameObjects.clear();
	m_all.clear();
	m_goPointers.clear();
	m_goKeys.clear();
}

// is only called if object comes from server and so it is never static
GameObject* GameController::spawnGameObject(std::string identifier, std::string key)
{
	GameObject* go = new GameObject(identifier, m_scenery.getObjectSetupMaps()[key]);
	m_goPointers[go->getIdentifier()] = go;
	m_goKeys[go->getIdentifier()] = key;
	m_gameObjects.push_back(go);
	m_scene.Add(go->getBody());
	m_all.push_back(go);
	return go;
}

GameObject* GameController::spawnGameObject(std::string key, bool isStatic)
{
	GameObject* go = new GameObject(m_scenery.getObjectSetupMaps()[key]);
	m_goPointers[go->getIdentifier()] = go;
	m_goKeys[go->getIdentifier()] = key;
	if(isStatic)
		m_staticGameObjects.push_back(go);
	else
		m_gameObjects.push_back(go);
	m_scene.Add(go->getBody());
	m_all.push_back(go);
	return go;
}

GameObject* GameController::spawnPlayer(std::string identifier, std::string key)
{
	GameObject* go = new GameObject(identifier, m_scenery.getPlayerSetupMaps()[key]);
	m_goPointers[go->getIdentifier()] = go;
	m_goKeys[go->getIdentifier()] = key;
	m_players.push_back(go);
	m_scene.Add(go->getBody());
	m_all.push_back(go);
	return go;
}

// is only called from lobby and so always a remotePlayer
GameObject* GameController::spawnPlayer(std::string key)
{
	GameObject* go = new GameObject(m_scenery.getPlayerSetupMaps()[key]);
	m_goPointers[go->getIdentifier()] = go;
	m_goKeys[go->getIdentifier()] = key;
	m_players.push_back(go);
	m_scene.Add(go->getBody());
	m_all.push_back(go);
	return go;
}

void GameController::spawnStaticGameObjects()
{
	for(auto& key : m_scenery.getStaticGameObjects()) {
		for(int i = 1; m_scenery.getSetupMap()[key].count(std::to_string(i)); i++){
			GameObject* go = spawnGameObject(key, true);
			go->setPos(Helper::toVector2f(m_scenery.getSetupMap()[key][std::to_string(i)]));
		}
	}
}

void GameController::spawnBeginningGameObjects()
{
	for(auto& key : m_scenery.getBeginningGameObjects()) {
		for(int i = 1; m_scenery.getSetupMap()[key].count(std::to_string(i)); i++){
			GameObject* go = spawnGameObject(key);
			go->setPos(Helper::toVector2f(m_scenery.getSetupMap()[key][std::to_string(i)]));
		}
	}
}

Net::GameObjectState GameController::getGameObjectState(GameObject* go)
{
		auto c = go->getConfig();
		return Net::GameObjectState{
			go->getIdentifier(),
			getGameObejctKey(go->getIdentifier()),
			c.name,
			c.visible,
			c.color,
			c.zindex,
			c.extensionMap,
			c.body.collidableSolid,
			c.body.collidableUnsolid,
			c.body.rotatable,
			c.body.solid,
			c.body.skip,
			c.body.x,
			c.body.y,
			c.body.orient,
			c.body.vx,
			c.body.vy,
			c.body.av
		};
}

void GameController::applyGameObjectState(GameObject* go, Net::GameObjectState gos)
{
	auto c = go->getConfig();
	c.name = gos.name;
	c.visible = gos.visible;
	c.color = gos.color;
	c.zindex = gos.zindex;
	c.extensionMap = gos.extensionMap;
	c.body.collidableSolid = gos.collidableSolid;
	c.body.collidableUnsolid = gos.collidableUnsolid;
	c.body.rotatable = gos.rotatable;
	c.body.solid = gos.solid;
	c.body.skip = gos.skip;
	c.body.x = gos.x;
	c.body.y = gos.y;
	c.body.orient = gos.orient;
	c.body.vx = gos.vx;
	c.body.vy = gos.vy;
	c.body.av = gos.av;
	go->applyConfig(c);
}

Net::GameState GameController::getGameState()
{
	Net::GameState gs;
	for(GameObject* go : getGameObjects()) {
		gs.objects.push_back(getGameObjectState(go));
	}
	for(GameObject* go : getPlayers()) {
		gs.players.push_back(getGameObjectState(go));
	}
	return gs;
}

void GameController::applyGameState(Net::GameState gs)
{
	for(Net::GameObjectState gos : gs.objects) {
		GameObject* go;
		try {
			go = getGameObejctPointer(gos.identifier);
		} catch(std::out_of_range& e) {
			go = spawnGameObject(gos.identifier, gos.key);
		}
		applyGameObjectState(go, gos);
	}
	for(Net::GameObjectState gos : gs.players) {
		GameObject* go;
		try {
			go = getGameObejctPointer(gos.identifier);
		} catch(std::out_of_range& e) {
			go = spawnPlayer(gos.identifier, gos.key);
		}
		applyGameObjectState(go, gos);
	}
}

void GameController::event(GameObject::Event event)
{
	for (GameObject* g : getAll()) {
		g->event(event);
	}
}

void GameController::update(sf::Time ellapsed) {
	m_clock += ellapsed;
	if(m_clock>=m_sceneDt) {
		// TODO fps from settings
		// use steady time for determenistic and to increase steadyness
		m_scene.Step(m_sceneDt.asSeconds(), ph::Vec2(0, getMap()->getGravity()));
		m_clock -= m_sceneDt;
	}

	for (GameObject* g : getAll()) {
		g->apply();
		g->update(ellapsed);
	}

	sortAll();
}

GameObject* GameController::getGameObejctPointer(std::string identifier)
{
	return m_goPointers.at(identifier);
}

std::string GameController::getGameObejctKey(std::string identifier)
{
	return m_goKeys.at(identifier);
}

Scenery& GameController::getScenery() {
	return m_scenery;
}

Map* GameController::getMap() {
	return m_scenery.getMap();
}

std::list<GameObject*>& GameController::getPlayers()
{
	return m_players;
}

std::list<GameObject*>& GameController::getStaticGameObjects()
{
	return m_staticGameObjects;
}

std::list<GameObject*>& GameController::getGameObjects()
{
	return m_gameObjects;
}

std::list<GameObject*>& GameController::getAll()
{
	return m_all;
}

const std::list<GameObject*>& GameController::getAll() const
{
	return m_all;
}

const ph::Scene& GameController::getScene() const
{
	return m_scene;
}

ph::Scene& GameController::getScene()
{
	return m_scene;
}

GameObject* GameController::getGameObjectById(const std::string & id) const
{
	for (GameObject* g : getAll())
	{
		if (g->getIdentifier() == id)
			return g;
	}
	return nullptr;
}

void GameController::updateLog() const
{
	for (GameObject* g : getAll()) {
		g->updateLog();
	}
}
