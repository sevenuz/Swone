 #include <game/GameController.h>

GameController::GameController(Controller& c) : m_controller(c) {}

GameController::~GameController() {
	// TODO move
	for (size_t i = 0; i < m_game_objects.size(); i++) {
		delete m_game_objects[i];
	}
}

Controller& GameController::getController() {
	return m_controller;
}

Map* GameController::getMap() {
	return m_map;
}

sf::View GameController::getView() {
	return m_view;
}

const std::vector<GameObject*>& GameController::getGameObjects() const{
	return m_game_objects;
}

void GameController::setViewCenter(sf::Vector2f pos) {
	m_view.setCenter(
		Map::toMapPixelX(pos.x) + m_viewDelta.x,
		Map::toMapPixelY(pos.y) + m_viewDelta.y
	);
}

void GameController::setMap(Map* m) {
	m_map = m;
	m_view = m_controller.getWindow().getView();
	m_view.zoom(1 / m->getScale());
}

void GameController::pushGameObject(GameObject* game_object) {
	m_game_objects.push_back(game_object);
}

void GameController::clearGameObjects()
{
	m_game_objects.clear();
}


void GameController::updateMap(sf::Time ellapsed) {
	getMap()->update(ellapsed);
}
void GameController::eventMap(sf::Event& e) {
	sf::RenderWindow& w = m_controller.getWindow();
	sf::Vector2i pixelPos = sf::Mouse::getPosition(w);
	sf::Vector2f worldPos = w.mapPixelToCoords(pixelPos);

	getMap()->event(e);
	// TODO get numbers from settings
	if (e.type == sf::Event::MouseWheelScrolled) // Zomm in or out if the mouse wheel moves
	{
		m_view.zoom(1.f + e.mouseWheelScroll.delta * 0.1f);
	}
	if (e.type == sf::Event::MouseMoved) // Move view
	{
		sf::Vector2f delta = worldPos - m_game_objects[0]->getPos();
		m_viewDelta.x = delta.x * 0.1f;
		m_viewDelta.y = delta.y * 0.1f;
	}
}

void GameController::updateGameObjects(sf::Time ellapsed) {
	for (GameObject* g : m_game_objects) {
		g->calculatePos(ellapsed);
		g->calculateVel(ellapsed, m_map->getGravity());
		sf::Vector2f& pos = g->getNextPos();

		sf::Vector2f hbrb = g->getHitboxRightBottom(pos);
		sf::Vector2f hblb = g->getHitboxLeftBottom(pos);
		sf::Vector2f hbrt = g->getHitboxRightTop(pos);
		sf::Vector2f hblt = g->getHitboxLeftTop(pos);
		const Tile& tile_rb = m_map->getTile(round(hbrb.y), round(hbrb.x));
		const Tile& tile_lb = m_map->getTile(round(hblb.y), round(hblb.x));
		const Tile& tile_rt = m_map->getTile(round(hbrt.y), round(hbrt.x));
		const Tile& tile_lt = m_map->getTile(round(hblt.y), round(hblt.x));
		g->onTiles(tile_lt, tile_rt, tile_lb, tile_rb);

		setViewCenter(g->getPos());
		g->update(ellapsed);
	}
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
