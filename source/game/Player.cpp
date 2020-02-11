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

bool Player::isMoving() {
    return m_isMoving;
}

bool Player::isJumping() {
    return m_isJumping;
}

bool Player::isFalling() {
    return m_isFalling;
}

void Player::toggleFalling(bool falling) {
    m_isFalling = falling;
    m_isJumping = !falling && m_vec.y < 0;
}

void Player::stopFalling() {
    toggleFalling(false);
}

void Player::startFalling() {
    toggleFalling(true);
}

void Player::jump(){
	if(m_jumps > 0){
		m_jumps--;
		m_vec.y = -1 * m_jf; // Startkraft nach oben
		toggleFalling(false);
	}
}

void Player::move(float fx){
	m_vec.x = fx;
	m_isMoving = fx != 0;
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

sf::Vector2f& Player::calculatePos(sf::Time ellapsed) {
    const float s = ellapsed.asSeconds();
    m_nextPos.x = m_pos.x + (m_vec.x * SPEED_FACTOR * s);
    m_nextPos.y = m_pos.y + (m_vec.y * SPEED_FACTOR * s);
    return m_nextPos;
}

sf::Vector2f& Player::calculateVec(sf::Time ellapsed, sf::Vector2f newPos) {
    const float s = ellapsed.asSeconds();
    const float g = GRAVITY;
    float fx = m_vec.x;
    // x linear, y beschleunigt
    float fy = m_vec.y  + (m_m * g * s);
    // TODO
    if(fy > g)
        fy = g;
    m_nextVec.x = fx;
    m_nextVec.y = fy;
    return m_nextVec;
}

void Player::apply() {
    setPos(m_nextPos);
    setVec(m_nextVec);
}

void Player::applyX() {
    m_nextPos.y = m_pos.y;
    apply();
}

void Player::applyY() {
    m_nextPos.x = m_pos.x;
    apply();
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

void Player::resetJump(){
	m_jumpCooldown = sf::seconds(0);
	m_jumps = m_jumpsPossible;
}

void Player::setPos(sf::Vector2f pos) {
    m_pos = pos;
    m_sprite.setPosition(Map::toMapPixelX(m_pos.x), Map::toMapPixelY(m_pos.y));
}

void Player::setVec(sf::Vector2f vec) {
    m_vec.x = vec.x;
    m_vec.y = vec.y;
}

sf::Vector2f Player::getSpritePos() {
    return m_sprite.getPosition();
}

void Player::onTiles(unsigned int leftTop, unsigned int rightTop, unsigned int leftBottom, unsigned int rightBottom) {
    if(leftBottom != MapTiles::SPACE || rightBottom != MapTiles::SPACE) {
        stopFalling();
        resetJump();
        applyX();
    } else {
        startFalling();
        apply();
    }
}

void Player::onOutOfMap() {
    std::cout << "oh noo!" << std::endl;
    apply();
};

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	target.draw(m_sprite, states);
}

