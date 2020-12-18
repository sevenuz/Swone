#ifndef SWONE_GAME_GAMECONTROLLER_H
#define SWONE_GAME_GAMECONTROLLER_H

#include <list>

#include "game/Map.h"
#include "game/object/GameObject.h"
#include "util/Log.h"
#include "physics/Scene.h"

class GameController {
public:
	GameController();
	virtual ~GameController();

	Map* getMap();
	const std::list<GameObject*>& getGameObjects() const;
	GameObject* getGameObjectById(const std::string& id) const;

	void updateLog() const;

	void updateMap(sf::Time ellapsed);
	void eventMap(sf::Event& e);

	void updateGameObjects(sf::Time ellapsed);
	void eventGameObjects(sf::Event& e);

	void setMap(Map* m);
	void startGame();

	void pushGameObject(GameObject* game_object);
	void clearGameObjects();

	const ph::Scene& getScene() const;
	ph::Scene& getScene();
protected:
private:
	Map* m_map = NULL;
	ph::Scene m_scene;
	sf::Time m_clock;
	sf::Time m_sceneDt = sf::seconds(1.0f/60.0f);
	std::list<GameObject*> m_game_objects;

	size_t round(float f) {
		return static_cast<size_t> (f);
	}
};
#endif

