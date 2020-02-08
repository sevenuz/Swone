#include <game/GameController.h>


GameController::GameController(Controller & c): m_controller(c){
	Player * p = new Player(3 , 2.7);
	pushPlayer(p);
}

GameController::~GameController(){
	for(unsigned int i = 0; i < m_player.size(); i++){
			delete m_player[i];
	}
}

Controller& GameController::getController(){
	return m_controller;
}

Map * GameController::getMap(){
	return m_map;
}

sf::View GameController::getView() {
    return m_view;
}

std::vector<Player *> GameController::getPlayers(){
	return m_player;
}

void GameController::setMap(Map * map){
	m_map = map;
	m_view = m_controller.getWindow().getView();
	m_view.zoom(1/map->getScale());
}

void GameController::pushPlayer(Player * player){
	m_player.push_back(player);
}

void GameController::updateMap(sf::Time ellapsed){
	getMap()->update(ellapsed);
}
void GameController::eventMap(sf::Event& e){
    sf::RenderWindow & w = m_controller.getWindow();
	sf::Vector2i pixelPos = sf::Mouse::getPosition(w);
	sf::Vector2f worldPos = w.mapPixelToCoords(pixelPos);

	getMap()->event(e);
	if(e.type == sf::Event::MouseWheelMoved) // Zomm in or out if the mouse wheel moves
      {
        m_view.zoom(1.f+e.mouseWheel.delta*0.1f);
      }
    if(e.type == sf::Event::MouseMoved) // Zomm in or out if the mouse wheel moves
      {
        sf::Vector2f delta = worldPos - m_view.getCenter();
        m_view.move(delta.x*0.01, delta.y*0.01f);
      }
}

void GameController::updatePlayers(sf::Time ellapsed){
	for(unsigned int i = 0; i < getPlayers().size(); i++){
	    Player * p = getPlayers()[i];
	    sf::Vector2f pos = p->calculatePos(ellapsed);
	    unsigned int tile = m_map->getMapDataValue(round(pos.x), round(pos.y));
	    m_controller.pushLogMsg(tile);
	    m_controller.pushLogMsg(round(pos.x));
	    //m_controller.pushLogMsg(round(pos.y));
	    sf::Vector2f vec = p->calculateVec(ellapsed, pos);
	    if(pos.y >= m_map->getHeight() - 1){
	        pos.y = m_map->getHeight() - 2;
	        vec.y = 0;
	    }
	    p->setPos(pos);
	    p->setVec(vec);
		p->update(ellapsed);
	}
}
void GameController::eventPlayers(sf::Event& e){
	for(unsigned int i = 0; i < getPlayers().size(); i++){
		getPlayers()[i]->event(e);
	}
}

