#ifndef SWONE_CLIENT_GAME_CLIENT_GAME_CONTROLLER_H
#define SWONE_CLIENT_GAME_CLIENT_GAME_CONTROLLER_H

#include <map>

#include "client/Settings.h"
#include "game/GameController.h"

class ClientGameController : public GameController {
public:
	ClientGameController();
	virtual ~ClientGameController();

	void clearAll() override;
	void removeFromGame(GameObject* go) override;

	GameObject* spawnLocalPlayer(std::string identifier, std::string key);
	std::list<GameObject*>& getLocalPlayers();

	void setLocalPlayerKeybinding(GameObject* go, Settings::Keybinding);
	const Settings::Keybinding& getLocalPlayerKeybinding(GameObject* go);
private:
	std::list<GameObject*> m_localPlayers; // subset of GameController::m_players and m_all
	std::map<GameObject*, Settings::Keybinding> m_playerKeybindings;
};

#endif
