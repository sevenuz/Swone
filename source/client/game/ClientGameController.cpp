#include "game/object/GameObject.h"
#include <client/game/ClientGameController.h>

ClientGameController::ClientGameController()
{}

ClientGameController::~ClientGameController()
{
	clearAll();
	delete m_mapDrawing;
}

void ClientGameController::removeFromGame(GameObject* go)
{
	GameObjectDrawing* god = m_goDrawingsMap[go];
	m_goDrawings.remove(god);
	m_goDrawingsMap.erase(go);
	delete god;

	m_localPlayers.remove(go);
	GameController::removeFromGame(go);
}

void ClientGameController::clearAll()
{
	for(GameObjectDrawing* god : m_goDrawings) {
		delete god;
	}
	m_goDrawings.clear();
	m_goDrawingsMap.clear();

	GameController::clearAll();
	m_localPlayers.clear();
}

void ClientGameController::loadScenery(std::string resDir, Net::GameFileCheck gfc)
{
	GameController::loadScenery(resDir, gfc);
	delete m_mapDrawing;
	m_mapDrawing = new MapDrawing(*getScenery().getMap());
}

void ClientGameController::update(sf::Time ellapsed)
{
	GameController::update(ellapsed);
	for(GameObjectDrawing* god : m_goDrawings)
		god->update(ellapsed);
	sortGameObjectDrawings();
}

void ClientGameController::sortGameObjectDrawings()
{
	// sort list with z-index
	m_goDrawings.sort([](GameObjectDrawing* a, GameObjectDrawing* b) -> bool {
		return a->getGameObject().getZindex() < b->getGameObject().getZindex();
	});
}

GameObject* ClientGameController::spawnGameObject(std::string identifier, std::string key)
{
	GameObject* go = GameController::spawnGameObject(identifier, key);
	GameObjectDrawing* god = new GameObjectDrawing(
		*go,
		getScenery().getObjectSetupMaps()[key],
		[&](GameObject* go) -> GameObjectDrawing* { return m_goDrawingsMap[go]; }
	);
	m_goDrawings.push_back(god);
	m_goDrawingsMap[go] = god;
	return go;
}

GameObject* ClientGameController::spawnPlayer(std::string identifier, std::string key)
{
	GameObject* go = GameController::spawnPlayer(identifier, key);
	GameObjectDrawing* god = new GameObjectDrawing(
		*go,
		getScenery().getPlayerSetupMaps()[key],
		[&](GameObject* go) -> GameObjectDrawing* { return m_goDrawingsMap[go]; }
	);
	m_goDrawings.push_back(god);
	m_goDrawingsMap[go] = god;
	return go;
}

GameObject* ClientGameController::spawnLocalPlayer(std::string identifier, std::string key)
{
	GameObject* go = spawnPlayer(identifier, key);
	m_localPlayers.push_back(go);
	return go;
}

std::list<GameObject*>& ClientGameController::getLocalPlayers()
{
	return m_localPlayers;
}

void ClientGameController::setLocalPlayerKeybinding(GameObject* go, Settings::Keybinding k)
{
	m_playerKeybindings[go] = k;
}

const Settings::Keybinding& ClientGameController::getLocalPlayerKeybinding(GameObject* go)
{
	return m_playerKeybindings.at(go);
}

const std::list<GameObjectDrawing*> ClientGameController::getGameObjectDrawings() const
{
	return m_goDrawings;
}

const MapDrawing& ClientGameController::getMapDrawing() const
{
	return *m_mapDrawing;
}
