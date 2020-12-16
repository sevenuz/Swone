 #include <game/GameController.h>

GameController::GameController(Controller& c) : m_controller(c) {}

GameController::~GameController() {}

Controller& GameController::getController() {
	return m_controller;
}

Map* GameController::getMap() {
	return m_map;
}

sf::View GameController::getView() {
	return m_view;
}

const std::list<GameObject*>& GameController::getGameObjects() const{
	return m_game_objects;
}

void GameController::setViewCenter(sf::Vector2f pos) {
	m_view.setCenter(
		Map::toMapPixelX(pos.x) + m_viewDelta.x,
		Map::toMapPixelY(pos.y) + m_viewDelta.y
	);
}

sf::Vector2f GameController::getPlayerCenter()
{
	sf::Vector2f ppos = sf::Vector2f(0,0);
	size_t pcount = 0;
	for (GameObject* g : m_game_objects) {
		if(g->getType() == GameObject::S_PLAYER_TYPE) {
			ppos += g->getPos();
			pcount++;
		}
	}
	ppos.x /= pcount;
	ppos.y /= pcount;
	return ppos;
}

void GameController::setMap(Map* m) {
	m_map = m;
	m_view = m_controller.getWindow().getView();
	m_view.zoom(1 / m->getScale());
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

void GameController::updateMap(sf::Time ellapsed) {
	m_clock += ellapsed;
	if(m_clock>=m_sceneDt) {
		// TODO fps from settings
		// use steady time for determenistic and to increase steadyness
		m_scene.Step(m_sceneDt.asSeconds(), ph::Vec2(0, m_map->getGravity()));
		m_clock -= m_sceneDt;
	}
	setViewCenter(getPlayerCenter());
	// TODO set View Zoom if player are far away from each other

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
		sf::Vector2f delta = worldPos - getPlayerCenter();
		m_viewDelta.x = delta.x * 0.1f;
		m_viewDelta.y = delta.y * 0.1f;
	}
	// TODO remove shape spawn
	if (e.type == sf::Event::MouseButtonPressed)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			ph::PolygonShape poly;
			ph::uint32 count = (ph::uint32)ph::Random( 3, MaxPolyVertexCount );
			ph::Vec2 *vertices = new ph::Vec2[count];
			ph::real e = ph::Random( 5*0.05, 10*0.05 );
			for(ph::uint32 i = 0; i < count; ++i)
				vertices[i].Set( ph::Random( -e, e ), ph::Random( -e, e ) );
			poly.Set( vertices, count );
			ph::Body *b = new ph::Body( ph::Body::Config{&poly, worldPos.x/64, worldPos.y/64} );
			b->SetOrient( ph::Random( -ph::PI, ph::PI ) );
			b->restitution = 0.2f;
			b->dynamicFriction = 0.2f;
			b->staticFriction = 0.4f;
			m_scene.Add(b);
			delete [] vertices;
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			ph::Circle c( ph::Random( 0.1f, 1.0f ) );
			ph::Body *b2 = new ph::Body( ph::Body::Config{&c, worldPos.x/64, worldPos.y/64} );
			m_scene.Add(b2);
		}
  }
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
