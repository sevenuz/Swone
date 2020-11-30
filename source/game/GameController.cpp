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

		sf::Vector2f left = g->getVel().y > 0 ? g->getHitboxLeftBottom(pos) : g->getHitboxLeftTop(pos);
		sf::Vector2f right = g->getVel().y > 0 ? g->getHitboxRightBottom(pos) : g->getHitboxRightTop(pos);
		const Tile& tile_left = m_map->getTile(round(left.y), round(left.x));
		const Tile& tile_right = m_map->getTile(round(right.y), round(right.x));
		g->onTilesY(tile_left, tile_right);

		sf::Vector2f top = g->getVel().x > 0 ? g->getHitboxRightTop(pos) : g->getHitboxLeftTop(pos);
		sf::Vector2f bottom = g->getVel().x > 0 ? g->getHitboxRightBottom(pos) : g->getHitboxLeftBottom(pos);
		const Tile& tile_top = m_map->getTile(round(top.y), round(top.x));
		const Tile& tile_bottom = m_map->getTile(round(bottom.y-0.001), round(bottom.x));
		g->onTilesX(tile_top, tile_bottom);

		g->apply();
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
