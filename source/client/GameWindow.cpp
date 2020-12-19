#include <client/GameWindow.h>

GameWindow::GameWindow(Controller& c, GameController& gc) : m_controller(c), m_gc(gc) {
	m_view = m_controller.getWindow().getView();
}

GameWindow::~GameWindow() {}

sf::View GameWindow::getView() {
	return m_view;
}

void GameWindow::setViewZoom() {
	float scaleW = (float)(m_controller.getSettings().getWidth()) / (float)(m_gc.getMap()->getImageWidth());
	float scaleH = (float)(m_controller.getSettings().getHeight()) / (float)(m_gc.getMap()->getImageHeight());

	float scale = (scaleW > scaleH) ? scaleH : scaleW;
	m_view.zoom(1 / scale);
}

void GameWindow::setViewCenter(sf::Vector2f pos) {
	m_view.setCenter(
		Map::toMapPixelX(pos.x) + m_viewDelta.x,
		Map::toMapPixelY(pos.y) + m_viewDelta.y
	);
}

sf::Vector2f GameWindow::getPlayerCenter()
{
	sf::Vector2f ppos = sf::Vector2f(0,0);
	size_t pcount = 0;
	for (GameObject* g : m_gc.getGameObjects()) {
		if(g->getType() == GameObject::S_PLAYER_TYPE) {
			ppos += g->getPos();
			pcount++;
		}
	}
	ppos.x /= pcount;
	ppos.y /= pcount;
	return ppos;
}

void GameWindow::update(sf::Time ellapsed) {
	m_gc.updateMap(ellapsed);
	m_gc.updateGameObjects(ellapsed);

	setViewCenter(getPlayerCenter());
	// TODO set View Zoom if player are far away from each other
}

void GameWindow::event(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			m_controller.setActiveGameWindow(ActiveGameWindow::MAPSELECTION);
			return;
		}
	}
	sf::RenderWindow& w = m_controller.getWindow();
	sf::Vector2i pixelPos = sf::Mouse::getPosition(w);
	sf::Vector2f worldPos = w.mapPixelToCoords(pixelPos);
		// TODO get numbers from settings
	if (event.type == sf::Event::MouseWheelScrolled) // Zomm in or out if the mouse wheel moves
	{
		m_view.zoom(1.f + event.mouseWheelScroll.delta * 0.1f);
	}
	if (event.type == sf::Event::MouseMoved) // Move view
	{
		sf::Vector2f delta = worldPos - getPlayerCenter();
		m_viewDelta.x = delta.x * 0.1f;
		m_viewDelta.y = delta.y * 0.1f;
	}
	// TODO remove shape spawn
	if (event.type == sf::Event::MouseButtonPressed)
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
			m_gc.getScene().Add(b);
			delete [] vertices;
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			ph::Circle c( ph::Random( 0.1f, 1.0f ) );
			ph::Body *b2 = new ph::Body( ph::Body::Config{&c, worldPos.x/64, worldPos.y/64} );
			m_gc.getScene().Add(b2);
		}
	}

	m_gc.eventMap(event);
	m_gc.eventGameObjects(event);
}

void GameWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.setView(m_view);

	bool drawMap = true;
	for (GameObject* g : m_gc.getGameObjects()) {
		if(g->getZindex() >= 0 && drawMap) {
			target.draw(*m_gc.getMap(), states);
			drawMap = false;
		}
		target.draw(*g, states);
	}
	if(drawMap)
		target.draw(*m_gc.getMap(), states);

	// TODO toggle hitboxes
	for (ph::Body* g : m_gc.getScene().bodies) {
		if(g->skip)
			continue;
		if(g->shape->GetType() == ph::Shape::Type::ePoly) {
			ph::PolygonShape* shape = reinterpret_cast<PHY_NS::PolygonShape*>(g->shape);
			size_t points = shape->m_vertexCount + 1;
			sf::VertexArray hb(sf::LineStrip, points);
			for(size_t i = 0; i<points; ++i) {
				ph::Vec2 v = shape->GetVertexPosition(i%(shape->m_vertexCount));
				float x = Map::toMapPixelX(v.x);
				float y = Map::toMapPixelY(v.y);
				hb[i].position = sf::Vector2f(x,y);
				hb[i].color = sf::Color::Red;
			}
			target.draw(hb, states);
		}
		else if(g->shape->GetType() == ph::Shape::Type::eCircle) {
			sf::CircleShape circle;
			circle.setOutlineColor(sf::Color::Red);
			circle.setFillColor(sf::Color::Transparent);
			circle.setOutlineThickness(1);
			float r = Map::toMapPixelX(g->shape->radius);
			float x = Map::toMapPixelX(g->position.x) - r;
			float y = Map::toMapPixelY(g->position.y) - r;
			circle.setRadius(r);
			circle.setPosition(x, y);
			target.draw(circle, states);
		}
	}
}
