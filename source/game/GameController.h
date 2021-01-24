#ifndef SWONE_GAME_GAMECONTROLLER_H
#define SWONE_GAME_GAMECONTROLLER_H

#include <vector>

#include "game/Net.h"
#include "game/Scenery.h"
#include "util/Log.h"
#include "physics/Scene.h"

class GameController {
public:
	GameController();
	virtual ~GameController();

	Scenery& getScenery();
	Map* getMap();
	const std::list<GameObject*>& getGameObjects() const;
	GameObject* getGameObjectById(const std::string& id) const;

	void updateLog() const;

	void update(sf::Time ellapsed);
	void event(sf::Event& e);

	void pushPlayer(GameObject* p);
	void clearPlayers();
	void startGame();

	void loadScenery(std::string resDir, Net::GameFileCheck gfc);

	const ph::Scene& getScene() const;
	ph::Scene& getScene();
protected:
private:
	Scenery m_scenery;
	std::vector<GameObject*> m_localPlayers;
	std::vector<GameObject*> m_remotePlayers;

	ph::Scene m_scene;
	sf::Time m_clock;
	sf::Time m_sceneDt = sf::seconds(1.0f/60.0f);

	size_t round(float f) {
		return static_cast<size_t> (f);
	}
};
#endif

