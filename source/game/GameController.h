#ifndef SWONE_GAME_GAMECONTROLLER_H
#define SWONE_GAME_GAMECONTROLLER_H

#include <map>
#include <list>

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
	std::list<GameObject*>& getStaticGameObjects();
	std::list<GameObject*>& getGameObjects();
	std::list<GameObject*>& getPlayers();
	GameObject* getGameObjectById(const std::string& id) const;

	void updateLog() const;

	virtual void update(sf::Time ellapsed);
	void event(GameObject::Event e);

	virtual void clearAll();
	virtual void removeFromGame(GameObject* go);

	virtual void loadScenery(std::string resDir, Net::GameFileCheck gfc);

	const ph::Scene& getScene() const;
	ph::Scene& getScene();

	virtual GameObject* spawnGameObject(std::string identifier, std::string key);
	virtual GameObject* spawnGameObject(std::string key, bool isStatic = false);
	virtual GameObject* spawnPlayer(std::string identifier, std::string key);
	virtual GameObject* spawnPlayer(std::string key);
	GameObject* getGameObejctPointer(std::string identifier);
	std::string getGameObejctKey(std::string identifier);

	void spawnStaticGameObjects();
	void spawnBeginningGameObjects();

	Net::GameObjectState getGameObjectState(GameObject* go);
	void applyGameObjectState(GameObject* go, Net::GameObjectState gos);
	Net::GameState getGameState();
	void applyGameState(Net::GameState gs);
protected:
private:
	Scenery m_scenery;
	std::list<GameObject*> m_players; // subset of m_all
	std::list<GameObject*> m_staticGameObjects; // subset of m_all, are not synced between players
	std::list<GameObject*> m_gameObjects; // subset of m_all
	std::list<GameObject*> m_all;

	std::map<std::string, GameObject*> m_goPointers; // associate gameobject id with the pointer
	std::map<std::string, std::string> m_goKeys; // associate gameobject id with the key of the setupMap

	ph::Scene m_scene;
	sf::Time m_clock;
	sf::Time m_sceneDt = sf::seconds(1.0f/60.0f);

	size_t round(float f) {
		return static_cast<size_t> (f);
	}
};
#endif

