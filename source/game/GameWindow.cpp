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
	target.draw(*m_gc.getMap(), states);
	for (GameObject* g : m_gc.getGameObjects()) {
		target.draw(*g, states);
	}
}
