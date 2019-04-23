#include <game/GameWindow.h>

GameWindow::GameWindow(GameController& gc): m_gc(gc) {

}

GameWindow::~GameWindow() {

}

void GameWindow::update(sf::Time ellapsed) {
	m_gc.updateMap(ellapsed);
	m_gc.updatePlayers(ellapsed);
}

void GameWindow::event(sf::Event& event) {
	if(event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			m_gc.getController().setActiveGameWindow(ActiveGameWindow::MAPSELECTION);
		}
	}
	m_gc.eventMap(event);
	m_gc.eventPlayers(event);
}

void GameWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	target.draw(* m_gc.getMap(), states);
	for(unsigned int i = 0; i < m_gc.getPlayers().size(); i++){
		target.draw(* m_gc.getPlayers()[i]->getAnimatedSprite(), states);
	}
}
