#include <game/GameWindow.h>

GameWindow::GameWindow(GameController& gc) : m_gc(gc) {

}

GameWindow::~GameWindow() {

}

void GameWindow::update(sf::Time ellapsed) {
	m_gc.updateMap(ellapsed);
	m_gc.updateGameObjects(ellapsed);
}

void GameWindow::event(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			m_gc.getController().setActiveGameWindow(ActiveGameWindow::MAPSELECTION);
		}
	}
	m_gc.eventMap(event);
	m_gc.eventGameObjects(event);
}

void GameWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	m_gc.getController().setView(m_gc.getView());

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
