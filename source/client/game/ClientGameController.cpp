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

	for(GameObject* go : getStaticGameObjects()) {
		GameObjectDrawing* god = new GameObjectDrawing(
			*go,
			getScenery().getObjectSetupMaps()[getGameObejctKey(go->getIdentifier())],
			[&](GameObject* go) -> GameObjectDrawing* { return m_goDrawingsMap[go]; }
		);
		m_goDrawings.push_back(god);
		m_goDrawingsMap[go] = god;
	}
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

ph::real interpolateVal(ph::real a, ph::real b, double percentage)
{
	ph::real c = a + ((b - a) * percentage);
	ph::real difference = c - a;
	ph::real sqr_difference = difference * difference;
	if(sqr_difference > LOWER_POSITION_THRESHOLD && sqr_difference < UPPER_POSITION_THRESHOLD) {
		a += difference * POSITION_CORRECTION;
	}
	return c;
}

void ClientGameController::interpolateGameObjectState(GameObject* go, Net::GameObjectState gos, const double percentage)
{
	auto c = go->getConfig();

	// only on state apply, else interpolation by physics
	if (percentage == 1.0) {
		gos.x = interpolateVal(c.body.x, gos.x, percentage);
		gos.y = interpolateVal(c.body.y, gos.y, percentage);
		gos.orient = interpolateVal(c.body.orient, gos.orient, percentage);
	}
	gos.vx = interpolateVal(c.body.vx, gos.vx, percentage);
	gos.vy = interpolateVal(c.body.vy, gos.vy, percentage);
	gos.av = interpolateVal(c.body.av, gos.av, percentage);

	applyGameObjectState(go, gos);
}

bool ClientGameController::isLocalPlayer(const std::string& id) {
	for(GameObject* p : getLocalPlayers())
		if(id == p->getIdentifier())
			return true;
	return false;
}

void ClientGameController::interpolateGameState(const Net::GameState& gs, const double percentage)
{
	for(Net::GameObjectState gos : gs.objects) {
		GameObject* go;
		try {
			go = getGameObejctPointer(gos.identifier);
		} catch(std::out_of_range& e) {
			go = spawnGameObject(gos.identifier, gos.key);
		}
		interpolateGameObjectState(go, gos, percentage);
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
		interpolateGameObjectState(go, gos, percentage);
	}
}

void ClientGameController::applyGameState(const Net::GameState& gs)
{
	interpolateGameState(gs, 1.0);
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
	// set empty event to compare with, the first time an event is polled for this player
	m_playerLastEvent[go] = GameObject::Event{};
	m_playerKeybindings[go] = k;
}

const Settings::Keybinding& ClientGameController::getLocalPlayerKeybinding(GameObject* go)
{
	return m_playerKeybindings.at(go);
}

void ClientGameController::setLocalPlayerLastEvent(GameObject* go, GameObject::Event e)
{
	m_playerLastEvent[go] = e;
}

const GameObject::Event& ClientGameController::getLocalPlayerLastEvent(GameObject* go)
{
	return m_playerLastEvent.at(go);
}

const std::list<GameObjectDrawing*> ClientGameController::getGameObjectDrawings() const
{
	return m_goDrawings;
}

const MapDrawing& ClientGameController::getMapDrawing() const
{
	return *m_mapDrawing;
}
