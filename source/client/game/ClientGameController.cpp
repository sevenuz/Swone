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

void ClientGameController::interpolateGameObjectState(GameObject* go, Net::GameObjectState gos)
{
	auto c = go->getConfig();
	ph::Vec2 posOld(c.body.x, c.body.y);
	ph::Vec2 posNew(gos.x, gos.y);
	ph::Vec2 difference = posNew - posOld;
	if(difference.LenSqr() > LOWER_POSITION_THRESHOLD && difference.LenSqr() < UPPER_POSITION_THRESHOLD) {
		posNew += difference * POSITION_CORRECTION;
		gos.x = posNew.x;
		gos.y = posNew.y;
	}
	ph::real oDifference = gos.orient - c.body.orient;
	if(oDifference > LOWER_ORIENT_THRESHOLD && oDifference < UPPER_ORIENT_THRESHOLD) {
		gos.orient += oDifference * ORIENT_CORRECTION;
	}
	applyGameObjectState(go, gos);
}

bool ClientGameController::isLocalPlayer(const std::string& id) {
	for(GameObject* p : getLocalPlayers())
		if(id == p->getIdentifier())
			return true;
	return false;
}

void ClientGameController::interpolateGameState(const Net::GameState& gs)
{
	for(Net::GameObjectState gos : gs.objects) {
		GameObject* go;
		try {
			go = getGameObejctPointer(gos.identifier);
		} catch(std::out_of_range& e) {
			go = spawnGameObject(gos.identifier, gos.key);
		}
		interpolateGameObjectState(go, gos);
	}
	for(Net::GameObjectState gos : gs.players) {
		if(isLocalPlayer(gos.identifier))
			continue;
		GameObject* go;
		try {
			go = getGameObejctPointer(gos.identifier);
		} catch(std::out_of_range& e) {
			go = spawnPlayer(gos.identifier, gos.key);
		}
		interpolateGameObjectState(go, gos);
	}
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
