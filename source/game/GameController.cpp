 #include <game/GameController.h>

GameController::GameController() {}

GameController::~GameController() {}

Map* GameController::getMap() {
	return m_map;
}

const std::list<GameObject*>& GameController::getGameObjects() const{
	return m_game_objects;
}

void GameController::setMap(Map* m) {
	m_map = m;
}

void GameController::startGame()
{
	m_scene.Clear();
	for(auto& mm : m_map->getMapData())
		for(auto& mmm : mm.second)
			if(!mmm.second->isPassable())
				m_scene.Add(mmm.second->body);
	for(GameObject* game_object : m_game_objects)
		m_scene.Add(game_object->getBody());
}

void GameController::pushGameObject(GameObject* game_object) {
	m_game_objects.push_back(game_object);
}

void GameController::clearGameObjects()
{
	m_game_objects.clear();
}

const ph::Scene& GameController::getScene() const
{
	return m_scene;
}

ph::Scene& GameController::getScene()
{
	return m_scene;
}

void GameController::update(sf::Time ellapsed) {
	m_clock += ellapsed;
	if(m_clock>=m_sceneDt) {
		// TODO fps from settings
		// use steady time for determenistic and to increase steadyness
		m_scene.Step(m_sceneDt.asSeconds(), ph::Vec2(0, m_map->getGravity()));
		m_clock -= m_sceneDt;
	}

	updateMap(ellapsed);
	updateGameObjects(ellapsed);
}

void GameController::updateMap(sf::Time ellapsed) {
	getMap()->update(ellapsed);
}

void GameController::eventMap(sf::Event& e) {
	getMap()->event(e);
}

void GameController::updateGameObjects(sf::Time ellapsed) {
	for (GameObject* g : m_game_objects) {
		g->apply();
		g->update(ellapsed);
	}
	// sort list with z-index
	m_game_objects.sort([](GameObject* a, GameObject* b) -> bool {
		return a->getZindex() < b->getZindex();
	});
}

void GameController::eventGameObjects(sf::Event& e) {
	for (GameObject* g : m_game_objects) {
		g->event(e);
	}
}

GameObject* GameController::getGameObjectById(const std::string & id) const
{
	for (GameObject* g : m_game_objects)
	{
		if (g->getIdentifier() == id)
			return g;
	}
	return nullptr;
}

void GameController::updateLog() const
{
	for (GameObject* g : m_game_objects) {
		g->updateLog();
	}
}
