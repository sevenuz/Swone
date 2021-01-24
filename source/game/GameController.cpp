#include <game/GameController.h>

GameController::GameController() {}

GameController::~GameController() {}

Scenery& GameController::getScenery() {
	return m_scenery;
}

Map* GameController::getMap() {
	return m_scenery.getMap();
}

const std::list<GameObject*>& GameController::getGameObjects() const {
	return m_scenery.getGameObjects();
}

void GameController::loadScenery(std::string resDir, Net::GameFileCheck gfc) {
	std::string path = GameReader::getFile(gfc.sceneryFile.second);
	m_scenery = Scenery(resDir, Helper::parseFileName(path), GameReader::getSceneryMap(path));
}

void GameController::pushPlayer(GameObject* p)
{
	m_scenery.getGameObjects().push_back(p);
}

void GameController::clearPlayers()
{
	m_localPlayers.clear();
	m_remotePlayers.clear();
}

void GameController::startGame()
{
	m_scene.Clear();
	for(auto& mm : getMap()->getMapData())
		for(auto& mmm : mm.second)
			if(!mmm.second->isPassable())
				m_scene.Add(mmm.second->body);
	for(GameObject* game_object : getGameObjects())
		m_scene.Add(game_object->getBody());
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
		m_scene.Step(m_sceneDt.asSeconds(), ph::Vec2(0, getMap()->getGravity()));
		m_clock -= m_sceneDt;
	}

	m_scenery.update(ellapsed);
}

void GameController::event(sf::Event& e) {
	m_scenery.event(e);
}

GameObject* GameController::getGameObjectById(const std::string & id) const
{
	for (GameObject* g : getGameObjects())
	{
		if (g->getIdentifier() == id)
			return g;
	}
	return nullptr;
}

void GameController::updateLog() const
{
	for (GameObject* g : getGameObjects()) {
		g->updateLog();
	}
}
