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
	const std::list<GameObject*>& getAll() const;
	std::list<GameObject*>& getAll();
	std::list<GameObject*>& getGameObjects();
	std::list<GameObject*>& getLocalPlayers();
	std::list<GameObject*>& getRemotePlayers();
	GameObject* getGameObjectById(const std::string& id) const;

	void updateLog() const;

	void update(sf::Time ellapsed);
	void event(sf::Event& e);

	void clearAll();

	void loadScenery(std::string resDir, Net::GameFileCheck gfc);

	const ph::Scene& getScene() const;
	ph::Scene& getScene();

	void removeFromGame(std::string identifier);

	void spawnGameObject(std::string identifier, std::string key);
	GameObject* spawnGameObject(std::string key);
	GameObject* spawnLocalPlayer(std::string identifier, std::string key);
	void spawnRemotePlayer(std::string identifier, std::string key);
	void spawnRemotePlayer(std::string key);
	void reset();
	void sortAll();

	void spawnStaticGameObjects();
	void spawnBeginningGameObjects();
protected:
private:
	Scenery m_scenery;
	std::list<GameObject*> m_localPlayers;
	std::list<GameObject*> m_remotePlayers;
	std::list<GameObject*> m_gameObjects;
	std::list<GameObject*> m_all;

	ph::Scene m_scene;
	sf::Time m_clock;
	sf::Time m_sceneDt = sf::seconds(1.0f/60.0f);

	size_t round(float f) {
		return static_cast<size_t> (f);
	}
};
#endif

