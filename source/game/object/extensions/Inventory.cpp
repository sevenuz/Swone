/*
 * Inventory.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */
#include "Inventory.h"

int Inventory::Inventory_count = 0;

Inventory::Inventory(GameObject* obj, std::map<std::string, StringMap>& setupMap) : Extension(obj), m_ps(100)
{
	if(setupMap.count(Extension::CONTROLS_PARAGRAPH)){
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(CONTROLS_INVENTORY_1))
			m_key_inv1 = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][CONTROLS_INVENTORY_1]);
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(CONTROLS_INVENTORY_2))
			m_key_inv2 = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][CONTROLS_INVENTORY_2]);
		if(setupMap[Extension::CONTROLS_PARAGRAPH].count(CONTROLS_INVENTORY_3))
			m_key_inv3 = (sf::Keyboard::Key)Helper::toInt(setupMap[Extension::CONTROLS_PARAGRAPH][CONTROLS_INVENTORY_3]);
	}

	m_ps.setColor(sf::Color::White);
	m_ps.setDrawingType(sf::Quads);
	m_ps.setLifetime(sf::seconds(1));
	m_ps.setOrigin(10,10,10,10, Origin::ON_BORDER);
	m_ps.setGravity(15,15);

	m_inventoryIndex = Inventory_count;
	Inventory_count++;
}

void Inventory::event(sf::Event& e)
{
	/*
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == m_key_inv1) {
			m_selection |= 0b1;
		}
		if (event.key.code == m_key_inv2) {
			m_selection |= 0b10;
		}
		if (event.key.code == m_key_inv3) {
			m_selection |= 0b100;
		}
	}
	else if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == m_key_inv1) {
			m_selection &= 0b1;
		}
		if (event.key.code == m_key_inv2) {
			m_selection &= 0b10;
		}
		if (event.key.code == m_key_inv3) {
			m_selection &= 0b100;
		}
	}
	*/
}

void Inventory::update(sf::Time ellapsed)
{
	m_ps.update(ellapsed);
}

void Inventory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.setView(target.getDefaultView());
	states.transform = sf::Transform::Identity;
	const float rectSize = 25;
	for(size_t j = 0; j < m_items.size(); j++) {
		sf::RectangleShape rectangle;
		rectangle.setPosition(sf::Vector2f(rectSize + m_inventoryIndex*m_items.size()*rectSize, j*rectSize));
		rectangle.setSize(sf::Vector2f(rectSize, rectSize));
		rectangle.setOutlineColor(m_obj->getColor());
		rectangle.setOutlineThickness(2);
		rectangle.setFillColor(sf::Color::Transparent);
		target.draw(rectangle, states);
	}
	target.draw(m_ps, states);
}
