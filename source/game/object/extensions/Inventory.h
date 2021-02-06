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

class InventoryDrawing;
class Inventory : public Extension {
	friend InventoryDrawing;
public:
	Inventory(GameObject* obj, StringMapMap& setupMap);

	void applyConfig(StringMapMap& setupMap) override;
	void getConfig(StringMapMap& setupMap) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void event(GameObject::Event e) override;
	void update(sf::Time ellapsed) override;
	void onObjectCollision(ph::Manifold* manifold, GameObject* go) override;

	void addObject(GameObject* g);
private:
	std::array<bool, INVENTORY_SIZE> m_selection = {};
	std::array<sf::RectangleShape*, INVENTORY_SIZE> m_rectangles;
	std::array<ParticleSystem*, INVENTORY_SIZE> m_particleSystems;
	std::array<GameObject*, INVENTORY_SIZE> m_items = {};
};

#endif
