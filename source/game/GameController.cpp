#include <game/GameController.h>

GameController::GameController() {}

GameController::~GameController() {}

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

void GameController::removeFromGame(std::string identifier)
{
	auto fn = [&](GameObject* go) -> bool {
		return go->getIdentifier() == identifier;
	};
	m_localPlayers.remove_if(fn);
	m_remotePlayers.remove_if(fn);
	m_gameObjects.remove_if(fn);
	m_all.remove_if([&](GameObject* go) -> bool {
		if(go->getIdentifier() == identifier) {
			m_scene.Remove(go->getBody());
			delete go;
			return true;
		} else {
			return false;
		}
	});
}

void GameController::spawnGameObject(std::string identifier, std::string key)
{
	GameObject* go = new GameObject(identifier, m_scenery.getObjectSetupMaps()[key]);
	m_gameObjects.push_back(go);
	m_scene.Add(go->getBody());
	m_all.push_back(go);
}

GameObject* GameController::spawnGameObject(std::string key)
{
	GameObject* go = new GameObject(m_scenery.getObjectSetupMaps()[key]);
	m_gameObjects.push_back(go);
	m_scene.Add(go->getBody());
	m_all.push_back(go);
	return go;
}

GameObject* GameController::spawnLocalPlayer(std::string identifier, std::string key)
{
	GameObject* go = new GameObject(identifier, m_scenery.getPlayerSetupMaps()[key]);
	m_localPlayers.push_back(go);
	m_scene.Add(go->getBody());
	m_all.push_back(go);
	return go;
}

void GameController::spawnRemotePlayer(std::string identifier, std::string key)
{
	GameObject* go = new GameObject(identifier, m_scenery.getPlayerSetupMaps()[key]);
	m_remotePlayers.push_back(go);
	m_scene.Add(go->getBody());
	m_all.push_back(go);
}

void GameController::spawnRemotePlayer(std::string key)
{
	GameObject* go = new GameObject(m_scenery.getPlayerSetupMaps()[key]);
	m_remotePlayers.push_back(go);
	m_scene.Add(go->getBody());
	m_all.push_back(go);
}

void GameController::spawnStaticGameObjects()
{
	for(auto& key : m_scenery.getStaticGameObjects()) {
		for(int i = 1; m_scenery.getSetupMap()[key].count(std::to_string(i)); i++){
			GameObject* go = spawnGameObject(key);
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

void GameController::clearAll()
{
	m_scene.Clear();
	for(GameObject* go : m_all)
		delete go;
	m_localPlayers.clear();
	m_remotePlayers.clear();
	m_gameObjects.clear();
	m_all.clear();
}

void GameController::event(sf::Event& event)
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

Scenery& GameController::getScenery() {
	return m_scenery;
}

Map* GameController::getMap() {
	return m_scenery.getMap();
}

std::list<GameObject*>& GameController::getLocalPlayers()
{
	return m_localPlayers;
}
std::list<GameObject*>& GameController::getRemotePlayers()
{
	return m_remotePlayers;
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
