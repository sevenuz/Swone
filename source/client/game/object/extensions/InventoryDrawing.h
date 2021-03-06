/*
 * InventoryDrawing.h
 *
 *  Created on: 05.02.2021
 *      Author: julius
 */

#ifndef SWONE_CLIENT_GAME_OBJECT_EXTENSIONS_INVENTORY_DRAWING_H
#define SWONE_CLIENT_GAME_OBJECT_EXTENSIONS_INVENTORY_DRAWING_H

#include <array>
#include <SFML/Graphics.hpp>

#include "graphics/ParticleSystem.h"

#include "util/reader/Reader.h"
#include "client/game/object/ExtensionDrawing.h"
#include "game/object/extensions/Inventory.h"

#define INVENTORY_WIDTH 40
#define INVENTORY_HEIGHT 40

class InventoryDrawing : public ExtensionDrawing {
public:
	static int Inventory_count;

	InventoryDrawing(const Inventory& inv, StringMapMap& setupMap, std::function<GameObjectDrawing*(GameObject*)> goGodMapper);
	virtual ~InventoryDrawing();

	void applyConfig(StringMapMap& setupMap) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void update(sf::Time ellapsed) override;
private:
	std::function<GameObjectDrawing*(GameObject*)> m_goGodMapper;
	const Inventory& m_inventory;
	std::array<sf::RectangleShape*, INVENTORY_SIZE> m_rectangles;
	std::array<ParticleSystem*, INVENTORY_SIZE> m_particleSystems;
};

#endif
