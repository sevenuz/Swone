/*
 * Inventory.cpp
 *
 *  Created on: 14.09.2018
 *      Author: julius
 */
#include "Inventory.h"

Inventory::Inventory(GameObject* obj, StringMapMap& setupMap) : Extension(obj)
{
	applyConfig(setupMap);
}

void Inventory::applyConfig(StringMapMap& setupMap)
{
	for(size_t i = 0; i < INVENTORY_SIZE; ++i) {
		if(setupMap[S_INVENTORY_ITEMS].count(std::to_string(i))) {
			m_items[i] = getGameObjectByIdentifier(setupMap[S_INVENTORY_ITEMS][std::to_string(i)]);
		} else {
			m_items[i] = nullptr;
		}
	}
}

void Inventory::getConfig(StringMapMap& extensionMap)
{
	for(size_t i = 0; i < INVENTORY_SIZE; ++i) {
		if(m_items[i]) {
			extensionMap[S_INVENTORY_ITEMS][std::to_string(i)] = m_items[i]->getIdentifier();
		}
	}
}

void Inventory::event(GameObject::Event e)
{
	if (e.action1) {
		m_selection[0] = true;
		if(m_items[0]) {
			Log::ger().log("activate " + m_items[0]->getName());
			m_items[0] = nullptr;
		}
	} else {
		m_selection[0] = false;
	}
	if (e.action2) {
		m_selection[1] = true;
		if(m_items[1]) {
			Log::ger().log("activate " + m_items[1]->getName());
			m_items[1] = nullptr;
		}
	} else {
		m_selection[1] = false;
	}
	if (e.action3) {
		m_selection[2] = true;
		if(m_items[2]) {
			Log::ger().log("activate " + m_items[2]->getName());
			m_items[2] = nullptr;
		}
	} else {
		m_selection[2] = false;
	}
}

bool Inventory::addObject(GameObject* g)
{
	// TODO why required?? because physics call collision callback multiple times?
	for(size_t i = 0; i < INVENTORY_SIZE; ++i)
		if(m_items[i])
			if(m_items[i]->getIdentifier() == g->getIdentifier()) {
				Log::ger().log("already in Inventory");
				return false;
			}

	for(size_t i = 0; i < INVENTORY_SIZE; ++i) {
		if(!m_items[i]) {
			m_items[i] = g;
			g->getBody()->skip = true;
			g->setVisible(false);
			return true;
		}
	}
	Log::ger().log("inventory is full");
	return false;
}

void Inventory::onObjectCollision(ph::Manifold* manifold, GameObject* go)
{
	if(go->getType() != GameObject::S_ITEM_TYPE)
		return;
	if(addObject(go)) {
	}
}
