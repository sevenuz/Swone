#include "server/Player.h"

Player::Player(sf::Time lpt, Player::Connection c) : m_lastPackTime(lpt), m_connection(c) {}

Player::~Player() {}

Player::Connection& Player::getConnection()
{
	return m_connection;
}
