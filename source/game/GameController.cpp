#include <game/GameController.h>


GameController::GameController(Controller & c): m_controller(c){
	Player * p = new Player(0 , 100);
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

std::vector<Player *> GameController::getPlayers(){
	return m_player;
}

void GameController::setMap(Map * map){
	m_map = map;
}

void GameController::pushPlayer(Player * player){
	m_player.push_back(player);
}

void GameController::updateMap(sf::Time ellapsed){
	getMap()->update(ellapsed);
}
void GameController::eventMap(sf::Event& e){
	getMap()->event(e);
}

void GameController::updatePlayers(sf::Time ellapsed){
	for(unsigned int i = 0; i < getPlayers().size(); i++){
		getPlayers()[i]->update(ellapsed);
	}
}
void GameController::eventPlayers(sf::Event& e){
	for(unsigned int i = 0; i < getPlayers().size(); i++){
		getPlayers()[i]->event(e);
	}
}
