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
	// TODO read inventory from extensionMap?
}

void Inventory::getConfig(StringMapMap& extensionMap)
{
	// TODO write inventory to extensionMap?
}

void Inventory::event(GameObject::Event e)
{
	if (e.action1) {
		m_selection[0] = true;
		if(m_items[0])
			Log::ger().log("activate " + m_items[0]->getName());
	} else {
		m_selection[0] = false;
	}
	if (e.action2) {
		m_selection[1] = true;
		if(m_items[1])
			Log::ger().log("activate " + m_items[1]->getName());
	} else {
		m_selection[1] = false;
	}
	if (e.action3) {
		m_selection[2] = true;
		if(m_items[2])
			Log::ger().log("activate " + m_items[2]->getName());
	} else {
		m_selection[2] = false;
	}
}

bool Inventory::addObject(GameObject* g)
{
	for(size_t i = 0; i < INVENTORY_SIZE; ++i) {
		if(m_items[i] == NULL) {
			m_items[i] = g;
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
		go->getBody()->skip = true;
		go->setVisible(false);
	}
}
