#include <client/game/ClientGameController.h>

ClientGameController::ClientGameController()
{}

ClientGameController::~ClientGameController()
{}

void ClientGameController::removeFromGame(GameObject* go)
{
	m_localPlayers.remove(go);
	GameController::removeFromGame(go);
}

void ClientGameController::clearAll()
{
	GameController::clearAll();
	m_localPlayers.clear();
}

GameObject* ClientGameController::spawnLocalPlayer(std::string identifier, std::string key)
{
	GameObject* go = spawnPlayer(identifier, key);
	m_localPlayers.push_back(go);
	return go;
}

std::list<GameObject*>& ClientGameController::getLocalPlayers()
{
	return m_localPlayers;
}

void ClientGameController::setLocalPlayerKeybinding(GameObject* go, Settings::Keybinding k)
{
	m_playerKeybindings[go] = k;
}

const Settings::Keybinding& ClientGameController::getLocalPlayerKeybinding(GameObject* go)
{
	return m_playerKeybindings.at(go);
}
