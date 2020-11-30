/*
 * Inventory.h
 *
 *  Created on: 26.11.2020
 *      Author: julius
 */

#ifndef SWONE_GAME_OBJECT_EXTENSIONS_INVENTORY_H
#define SWONE_GAME_OBJECT_EXTENSIONS_INVENTORY_H

#include <array>
#include <SFML/Graphics.hpp>

#include "graphics/ParticleSystem.h"

#include "util/reader/Reader.h"
#include "game/object/Extension.h"

#define INVENTORY_SIZE 3

#define INVENTORY_WIDTH 40
#define INVENTORY_HEIGHT 40

class Inventory : public Extension {
public:
	// in controls paragraph
	static constexpr const char* CONTROLS_INVENTORY_1 = "inventory1";
	static constexpr const char* CONTROLS_INVENTORY_2 = "inventory2";
	static constexpr const char* CONTROLS_INVENTORY_3 = "inventory3";

	Inventory(GameObject* obj, std::map<std::string, StringMap>& setupMap);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void event(sf::Event& e) override;
	void update(sf::Time ellapsed) override;

	static int Inventory_count;
private:
	sf::Keyboard::Key m_key_inv1 = sf::Keyboard::Comma;
	sf::Keyboard::Key m_key_inv2 = sf::Keyboard::Period;
	sf::Keyboard::Key m_key_inv3 = sf::Keyboard::Hyphen;

	std::array<bool, INVENTORY_SIZE> m_selection = {};
	std::array<sf::RectangleShape*, INVENTORY_SIZE> m_rectangles;
	std::array<ParticleSystem*, INVENTORY_SIZE> m_particleSystems;
	std::array<GameObject*, INVENTORY_SIZE> m_items = {};
};

#endif
