/*
 * Inventory.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */
#include "Inventory.h"

int Inventory::Inventory_count = 0;

Inventory::Inventory(GameObject* obj, std::map<std::string, StringMap>& setupMap) : Extension(obj)
{
	Inventory_count++;

	sf::Vector2f sizeVct = sf::Vector2f(INVENTORY_WIDTH, INVENTORY_HEIGHT);
	for(size_t i = 0; i < INVENTORY_SIZE; ++i) {
		sf::Vector2f posVct = sf::Vector2f(1 + i * INVENTORY_WIDTH, Inventory_count * INVENTORY_SIZE * INVENTORY_HEIGHT);

		sf::RectangleShape* r = new sf::RectangleShape(sizeVct);
		r->setPosition(posVct);
		r->setOutlineThickness(2);
		r->setFillColor(sf::Color::Transparent);
		m_rectangles[i] = r;

		ParticleSystem* ps = new ParticleSystem(50);
		ps->setColor(sf::Color::White);
		ps->setDrawingType(sf::Quads);
		ps->setLifetime(sf::seconds(1));
		ps->setOrigin(posVct, sizeVct, Origin::ON_BORDER);
		m_particleSystems[i] = ps;
	}

	applyConfig(setupMap);
}

void Inventory::applyConfig(std::map<std::string, StringMap>& setupMap)
{
	if(setupMap.count(Extension::CONTROLS_PARAGRAPH)){
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(CONTROLS_INVENTORY_1))
			m_key_inv1 = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][CONTROLS_INVENTORY_1]);
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(CONTROLS_INVENTORY_2))
			m_key_inv2 = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][CONTROLS_INVENTORY_2]);
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(CONTROLS_INVENTORY_3))
			m_key_inv3 = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][CONTROLS_INVENTORY_3]);
	}

	for(size_t i = 0; i < INVENTORY_SIZE; ++i)
		m_rectangles[i]->setOutlineColor(m_obj->getColor());
}

void Inventory::getConfig(std::map<std::string, StringMap>& extensionMap)
{
	// TODO write inventory to extensionMap?
}

void Inventory::event(sf::Event& e)
{
	if (e.type == sf::Event::KeyPressed) {
		if (e.key.code == m_key_inv1) {
			m_selection[0] = true;
			if(m_items[0])
				Log::ger().log("activate " + m_items[0]->getName());
		}
		if (e.key.code == m_key_inv2) {
			m_selection[1] = true;
			if(m_items[1])
				Log::ger().log("activate " + m_items[1]->getName());
		}
		if (e.key.code == m_key_inv3) {
			m_selection[2] = true;
			if(m_items[2])
				Log::ger().log("activate " + m_items[2]->getName());
		}
	}
	else if (e.type == sf::Event::KeyReleased) {
		if (e.key.code == m_key_inv1) {
			m_selection[0] = false;
		}
		if (e.key.code == m_key_inv2) {
			m_selection[1] = false;
		}
		if (e.key.code == m_key_inv3) {
			m_selection[2] = false;
		}
	}
}

void Inventory::update(sf::Time ellapsed)
{
	for(ParticleSystem* ps : m_particleSystems)
		ps->update(ellapsed);
}

void Inventory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	const sf::View gv = target.getView();

	target.setView(target.getDefaultView());
	for(size_t i = 0; i < INVENTORY_SIZE; ++i) {
		if(m_items[i] != NULL) {
			states.transform = sf::Transform::Identity * m_rectangles[i]->getTransform();
			states.transform.scale(m_items[i]->getSpriteScaleTo(sf::Vector2f(INVENTORY_WIDTH, INVENTORY_HEIGHT)));
			target.draw(*m_items[i]->getAnimatedSprite(), states);
		}
		states.transform = sf::Transform::Identity;
		target.draw(*m_rectangles[i], states);
		if(m_selection[i])
			target.draw(*m_particleSystems[i], states);
	}

	target.setView(gv);
}

void Inventory::addObject(GameObject* g)
{
	for(size_t i = 0; i < INVENTORY_SIZE; ++i) {
		if(m_items[i] == NULL) {
			m_items[i] = g;
			return;
		}
	}
	Log::ger().log("inventory is full");
}

void Inventory::onObjectCollision(ph::Manifold* manifold, GameObject* go)
{
	if(go->getType() == GameObject::S_PLAYER_TYPE)
		return;
	go->getBody()->skip = true;
	go->setVisible(false);
	addObject(go);
}
