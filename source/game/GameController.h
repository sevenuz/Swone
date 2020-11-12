#ifndef SWONE_GAME_GAMECONTROLLER_H
#define SWONE_GAME_GAMECONTROLLER_H

#include <vector>

#include "game/Map.h"
#include "game/Player.h"
#include "Controller.h"

class GameController {
public:
	GameController(Controller& c);
	virtual ~GameController();
	Controller& getController();

	Map* getMap();
	sf::View getView();
	std::vector<Player*> getPlayers();

	void updateMap(sf::Time ellapsed);
	void eventMap(sf::Event& e);

	// TODO should be GameObject functions
	void updatePlayers(sf::Time ellapsed);
	void eventPlayers(sf::Event& e);

	void setViewCenter(sf::Vector2f pos);

	void setMap(Map* m);
	// TODO should be GameObject fn
	void pushPlayer(Player*);
protected:
private:
	Controller& m_controller;
	Map* m_map = NULL;
	sf::Vector2f m_viewDelta = sf::Vector2f(0, 0);
	sf::View m_view;
	// TODO should be GameObject vector
	// multi dimensional -> back/forground
	std::vector<Player*> m_player;

	size_t round(float f) {
		return static_cast<size_t> (f);
	}
};
#endif

