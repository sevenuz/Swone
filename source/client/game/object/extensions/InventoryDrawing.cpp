/*
 * InventoryDrawing.cpp
 *
 *  Created on: 05.02.2021
 *      Author: julius
 */
#include "client/game/object/extensions/InventoryDrawing.h"

InventoryDrawing::InventoryDrawing(const Inventory& inv, StringMapMap& setupMap) : m_inventory(inv)
{
	sf::Vector2f sizeVct = sf::Vector2f(INVENTORY_WIDTH, INVENTORY_HEIGHT);
	for(size_t i = 0; i < INVENTORY_SIZE; ++i) {
		sf::Vector2f posVct = sf::Vector2f(1 + i * INVENTORY_WIDTH, Inventory::Inventory_count * INVENTORY_SIZE * INVENTORY_HEIGHT);

		sf::RectangleShape* r = new sf::RectangleShape(sizeVct); // TODO why allocate new?
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

void InventoryDrawing::applyConfig(StringMapMap& setupMap)
{
	for(size_t i = 0; i < INVENTORY_SIZE; ++i)
		m_rectangles[i]->setOutlineColor(m_inventory.m_obj->getColor());
}

void InventoryDrawing::update(sf::Time ellapsed)
{
	for(ParticleSystem* ps : m_particleSystems)
		ps->update(ellapsed);
}

void InventoryDrawing::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	const sf::View gv = target.getView();

	target.setView(target.getDefaultView());
	for(size_t i = 0; i < INVENTORY_SIZE; ++i) {
		if(m_inventory.m_items[i] != NULL) {
			states.transform = sf::Transform::Identity * m_rectangles[i]->getTransform();
			states.transform.scale(m_inventory.m_items[i]->getSpriteScaleTo(sf::Vector2f(INVENTORY_WIDTH, INVENTORY_HEIGHT)));
			target.draw(*m_inventory.m_items[i]->getAnimatedSprite(), states);
		}
		states.transform = sf::Transform::Identity;
		target.draw(*m_rectangles[i], states);
		if(m_inventory.m_selection[i])
			target.draw(*m_particleSystems[i], states);
	}

	target.setView(gv);
}
