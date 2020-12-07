#ifndef SWONE_GAME_GAMECONTROLLER_H
#define SWONE_GAME_GAMECONTROLLER_H

#include <vector>

#include "game/Map.h"
#include "game/object/GameObject.h"
#include "Controller.h"
#include "util/Log.h"
#include "physics/Scene.h"

class GameController {
public:
	GameController(Controller& c);
	virtual ~GameController();
	Controller& getController();

	Map* getMap();
	sf::View getView();
	const std::vector<GameObject*>& getGameObjects() const;
	GameObject* getGameObjectById(const std::string& id) const;

	void updateLog() const;

	void updateMap(sf::Time ellapsed);
	void eventMap(sf::Event& e);

	void updateGameObjects(sf::Time ellapsed);
	void eventGameObjects(sf::Event& e);

	void setViewCenter(sf::Vector2f pos);

	void setMap(Map* m);
	void startGame();

	void pushGameObject(GameObject* game_object);
	void clearGameObjects();

	ph::Scene getScene() const;
protected:
private:
	Controller& m_controller;
	Map* m_map = NULL;
	sf::Vector2f m_viewDelta = sf::Vector2f(0, 0);
	sf::View m_view;
	ph::Scene m_scene;
	sf::Time m_clock;
	sf::Time m_sceneDt = sf::seconds(1.0f/60.0f);
	// TODO multi dimensional -> back/forground
	std::vector<GameObject*> m_game_objects;

	size_t round(float f) {
		return static_cast<size_t> (f);
	}
};
#endif

