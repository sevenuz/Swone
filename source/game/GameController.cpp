
#include <game/GameController.h>


GameController::GameController(Controller& c) : m_controller(c) {}

GameController::~GameController() {
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

void GameController::setMap(Map* map) {
	m_map = map;
	m_view = m_controller.getWindow().getView();
	m_view.zoom(1 / map->getScale());
}

void GameController::pushGameObject(GameObject* game_object) {
	m_game_objects.push_back(game_object);
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
	// TODO remove
	if (e.type == sf::Event::KeyPressed) {
		if (e.key.code == sf::Keyboard::D) {
			GameObject* g = m_game_objects[0];
			sf::Vector2f pos = g->getHitboxRightBottom(g->getPos());
			MapTile tile = m_map->getMapDataValue(round(pos.y), round(pos.x));
			Log::ger().log("-------");
			Log::ger().log(tile);
			Log::ger().log(pos.x);
			Log::ger().log(pos.y);
		}
	}
}

void GameController::updateGameObjects(sf::Time ellapsed) {
	for (size_t i = 0; i < m_game_objects.size(); i++) {
		GameObject* g = m_game_objects[i];
		sf::Vector2f& pos = g->calculatePos(ellapsed);
		g->calculateVel(ellapsed, m_map->getGravity());

		if (pos.x > m_map->getWidth() || pos.x < 0 || pos.y > m_map->getHeight() || pos.y < 0) {
			g->onOutOfMap();
		}
		else {
			sf::Vector2f hbrb = g->getHitboxRightBottom(pos);
			sf::Vector2f hblb = g->getHitboxLeftBottom(pos);
			sf::Vector2f hbrt = g->getHitboxRightBottom(pos);
			sf::Vector2f hblt = g->getHitboxLeftBottom(pos);
			MapTile tile_rb = m_map->getMapDataValue(round(hbrb.y), round(hbrb.x));
			MapTile tile_lb = m_map->getMapDataValue(round(hblb.y), round(hblb.x));
			MapTile tile_rt = m_map->getMapDataValue(round(hbrt.y), round(hbrt.x));
			MapTile tile_lt = m_map->getMapDataValue(round(hblt.y), round(hblt.x));
			g->onTiles(tile_lt, tile_rt, tile_lb, tile_rb);
		}

		setViewCenter(g->getPos());
		g->update(ellapsed);
	}
}
void GameController::eventGameObjects(sf::Event& e) {
	for (size_t i = 0; i < m_game_objects.size(); i++) {
		m_game_objects[i]->event(e);
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
