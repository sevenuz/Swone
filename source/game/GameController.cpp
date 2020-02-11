#include <game/GameController.h>


GameController::GameController(Controller & c): m_controller(c){
	Player * p = new Player(0, 0);
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

void GameController::setViewCenter(sf::Vector2f pos) {
	m_view.setCenter(
	    Map::toMapPixelX(pos.x) + m_viewDelta.x,
	    Map::toMapPixelY(pos.y) + m_viewDelta.y
	);
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
        sf::Vector2f delta = worldPos - getPlayers()[0]->getPos();
        m_viewDelta.x = delta.x*0.1f;
        m_viewDelta.y = delta.y*0.1f;
      }
    if (e.type == sf::Event::KeyPressed) {
		if (e.key.code == sf::Keyboard::D) {
		    Player * p = getPlayers()[0];
	        sf::Vector2f& pos = p->getHitboxRightBotton();
	        unsigned int tile = m_map->getMapDataValue(round(pos.y), round(pos.x));
	        m_controller.pushLogMsg("-------");
	        m_controller.pushLogMsg(tile);
	        m_controller.pushLogMsg(pos.x);
	        m_controller.pushLogMsg(pos.y);
	    }
	}
}

void GameController::updatePlayers(sf::Time ellapsed){
	for(unsigned int i = 0; i < getPlayers().size(); i++){
	    Player * p = getPlayers()[i];
	    sf::Vector2f& pos = p->calculatePos(ellapsed);
	    sf::Vector2f& hbr = p->getHitboxRightBotton(pos);
	    sf::Vector2f& hbl = p->getHitboxLeftBotton(pos);
	    unsigned int tile_r = m_map->getMapDataValue(round(hbr.y), round(hbr.x));
	    unsigned int tile_l = m_map->getMapDataValue(round(hbl.y), round(hbl.x));
	    sf::Vector2f& vec = p->calculateVec(ellapsed, pos);

	    if(pos.y > m_map->getHeight()){
	        //TODO
	        //pos.y = m_map->getHeight();
	    }

        if(tile_r != MapTiles::SPACE || tile_l != MapTiles::SPACE) {
            p->stopFalling();
            p->resetJump();
            p->applyX();
        } else {
            p->startFalling();
            p->apply();
        }
        setViewCenter(p->getPos());
		p->update(ellapsed);
	}
}
void GameController::eventPlayers(sf::Event& e){
	for(unsigned int i = 0; i < getPlayers().size(); i++){
		getPlayers()[i]->event(e);
	}
}

