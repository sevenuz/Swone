#include "server/Player.h"

Player::Player(sf::Time lpt, Player::Connection c) : m_lastPackTime(lpt), m_connection(c) {}

Player::~Player() {}

std::string Player::createOrUpdatePlayer(GameController& gc, Net::PlayerConfigReq pcr)
{
	auto fn = [&](GameObject* go){
		go->setName(pcr.name);
		go->setColor(pcr.color);
	};
	for(GameObject* go : m_players) {
		if(go->getIdentifier() == pcr.identifier) {
			fn(go);
			return pcr.identifier;
		}
	}
	GameObject* go = gc.spawnPlayer(pcr.selection);
	fn(go);
	m_players.push_back(go);
	return go->getIdentifier();
}

Player::Connection& Player::getConnection()
{
	return m_connection;
}

std::vector<GameObject*>& Player::getPlayers()
{
	return m_players;
}
