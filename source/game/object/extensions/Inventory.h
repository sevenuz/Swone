/*
 * Inventory.h
 *
 *  Created on: 26.11.2020
 *      Author: julius
 */

#ifndef SWONE_GAME_OBJECT_EXTENSIONS_INVENTORY_H
#define SWONE_GAME_OBJECT_EXTENSIONS_INVENTORY_H

#include <array>

#include "util/reader/Reader.h"
#include "game/object/Extension.h"

#define INVENTORY_SIZE 3

class InventoryDrawing;
class Inventory : public Extension {
	friend InventoryDrawing;
public:
	Inventory(GameObject* obj, StringMapMap& setupMap);

	void applyConfig(StringMapMap& setupMap) override;
	void getConfig(StringMapMap& setupMap) override;

	void event(GameObject::Event e) override;
	void onObjectCollision(ph::Manifold* manifold, GameObject* go) override;

	bool addObject(GameObject* g);
private:
	std::array<bool, INVENTORY_SIZE> m_selection = {};
	std::array<GameObject*, INVENTORY_SIZE> m_items = {};
};

#endif
