#ifndef SWONE_CLIENT_GAME_CLIENT_GAME_CONTROLLER_H
#define SWONE_CLIENT_GAME_CLIENT_GAME_CONTROLLER_H

#include <map>

#include "client/Settings.h"
#include "client/game/object/GameObjectDrawing.h"
#include "client/game/MapDrawing.h"
#include "game/GameController.h"

class ClientGameController : public GameController {
public:
	ClientGameController();
	virtual ~ClientGameController();

	void clearAll() override;
	void removeFromGame(GameObject* go) override;

	void loadScenery(std::string resDir, Net::GameFileCheck gfc) override;
	void update(sf::Time ellapsed) override;
	void sortGameObjectDrawings();

	GameObject* spawnGameObject(std::string identifier, std::string key) override;
	GameObject* spawnPlayer(std::string identifier, std::string key) override;
	GameObject* spawnLocalPlayer(std::string identifier, std::string key);
	std::list<GameObject*>& getLocalPlayers();

	void setLocalPlayerKeybinding(GameObject* go, Settings::Keybinding);
	const Settings::Keybinding& getLocalPlayerKeybinding(GameObject* go);

	const std::list<GameObjectDrawing*> getGameObjectDrawings() const;
	const MapDrawing& getMapDrawing() const;
private:
	//GameObject* spawnGameObject(std::string key, bool isStatic = false) override;
	//GameObject* spawnPlayer(std::string key) override;

	std::list<GameObject*> m_localPlayers; // subset of GameController::m_players and m_all
	std::list<GameObjectDrawing*> m_goDrawings;
	std::map<GameObject*, GameObjectDrawing*> m_goDrawingsMap;
	std::map<GameObject*, Settings::Keybinding> m_playerKeybindings;

	MapDrawing* m_mapDrawing = nullptr;
};

#endif
