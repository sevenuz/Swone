/*
 * Player.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */

#include "Player.h"

Player::Player(float x, float y) {
	m_pos = sf::Vector2f(x, y);

	if (!m_texture.loadFromFile("../res/sprites/player.png")){
		std::cout << "Failed to load player spritesheet!" << std::endl;
	}

	m_ani_jump.setSpriteSheet(m_texture);
	m_ani_jump.addFrame(sf::IntRect(32, 0, 32, 32));
	m_ani_jump.addFrame(sf::IntRect(64, 0, 32, 32));
	m_ani_jump.addFrame(sf::IntRect(32, 0, 32, 32));
	m_ani_jump.addFrame(sf::IntRect( 0, 0, 32, 32));

	m_ani_left.setSpriteSheet(m_texture);
	m_ani_left.addFrame(sf::IntRect(32, 32, 32, 32));
	m_ani_left.addFrame(sf::IntRect(64, 32, 32, 32));
	m_ani_left.addFrame(sf::IntRect(32, 32, 32, 32));
	m_ani_left.addFrame(sf::IntRect( 0, 32, 32, 32));

	m_ani_right.setSpriteSheet(m_texture);
	m_ani_right.addFrame(sf::IntRect(32, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect(64, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect(32, 64, 32, 32));
	m_ani_right.addFrame(sf::IntRect( 0, 64, 32, 32));

	setAnimation(PlayerAnimation::LEFT);

	m_sprite.setPosition(sf::Vector2f(x, y));
	m_sprite.play(*m_ani);
}

Player::~Player() {
	// TODO Auto-generated destructor stub
}

sf::Vector2f Player::getLeftBoundry(){
	return m_left_boundry;
}

sf::Vector2f Player::getRightBoundry(){
	return m_right_boundry;
}

void Player::setAnimation(unsigned int ani){
	switch(ani) {
	case PlayerAnimation::LEFT:
		m_ani = &m_ani_left;
		break;
	case PlayerAnimation::RIGHT:
		m_ani = &m_ani_right;
		break;
	case PlayerAnimation::JUMP:
		m_ani = &m_ani_jump;
		break;
	}
}

AnimatedSprite * Player::getAnimatedSprite(){
	return &m_sprite;
}

void Player::jump(){
	if(m_jumps > 0){
		m_jumps--;
		m_vec.y = -1 * m_jf; // Startkraft nach oben
	}
}

void Player::move(float fx){
	m_vec.x = fx;
}

bool Player::isMoving() {
    return m_vec.x != 0 || m_vec.y != 0;
}

void Player::event(sf::Event& event) {
	if(event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Left) {
			setAnimation(PlayerAnimation::LEFT);
			move(-m_mf);
		} else if (event.key.code == sf::Keyboard::Right) {
			setAnimation(PlayerAnimation::RIGHT);
			move(m_mf);
		} else if (event.key.code == sf::Keyboard::Up) {
			setAnimation(PlayerAnimation::JUMP);
			jump();
		}
	} else if(event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::Left) {
			move(0);
		} else if (event.key.code == sf::Keyboard::Right) {
			move(0);
		}
	}
}

sf::Vector2f Player::calculatePos(sf::Time ellapsed) const {
    const float s = ellapsed.asSeconds();
    return sf::Vector2f(m_pos.x + (m_vec.x * s), m_pos.y + (m_vec.y * s));
}

sf::Vector2f Player::calculateVec(sf::Time ellapsed, sf::Vector2f newPos) const {
    const float s = ellapsed.asSeconds();
    const float deltaY = newPos.y - m_pos.y;
    // x linear, y beschleunigt
    // F=m*(2s/t**2)
    sf::Vector2f v = sf::Vector2f(m_vec.x, m_vec.y + (m_m * ((2*deltaY)/(s*s))));
    // TODO speedfactor
    v.x *= SPEED_FACTOR;
    v.y *= SPEED_FACTOR;
    return v;
}

void Player::update(sf::Time ellapsed){
	if(isMoving()){
		m_sprite.play(*m_ani);
		m_sprite.update(ellapsed);
	} else {
		m_sprite.stop();
	}
	refreshJump(ellapsed);
}

void Player::refreshJump(sf::Time ellapsed){
	m_jumpCooldown += ellapsed;
	if(m_jumpCooldown >= m_jumpCooldownTime){
		if(m_jumps < m_jumpsPossible){
			m_jumps++;
		}
		m_jumpCooldown = sf::seconds(0);
	}
}

void Player::setPos(sf::Vector2f pos) {
    m_pos = pos;
    pos.x *= Map::TILE_WIDTH;
    pos.y *= Map::TILE_HEIGHT;
    m_sprite.setPosition(pos);
}

void Player::setVec(sf::Vector2f vec) {
    m_vec = vec;
}

