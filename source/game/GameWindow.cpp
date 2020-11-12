#include <game/GameWindow.h>

GameWindow::GameWindow(GameController& gc) : m_gc(gc) {

}

GameWindow::~GameWindow() {

}

void GameWindow::update(sf::Time ellapsed) {
	m_gc.updateMap(ellapsed);
	m_gc.updatePlayers(ellapsed);
}

void GameWindow::event(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			m_gc.getController().setActiveGameWindow(ActiveGameWindow::MAPSELECTION);
		}
	}
	m_gc.eventMap(event);
	m_gc.eventPlayers(event);
}

void GameWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	m_gc.getController().setView(m_gc.getView());
	target.draw(*m_gc.getMap(), states);
	for (unsigned int i = 0; i < m_gc.getPlayers().size(); i++) {
		Player* p = m_gc.getPlayers()[i];
		target.draw(*p->getAnimatedSprite(), states);

		sf::FloatRect h = p->getHitboxBounds();
		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(h.width, h.height));
		rectangle.setOutlineColor(sf::Color::Red);
		rectangle.setOutlineThickness(1);
		rectangle.setFillColor(sf::Color::Transparent);
		rectangle.setPosition(h.left, h.top);
		target.draw(rectangle);
	}
}
