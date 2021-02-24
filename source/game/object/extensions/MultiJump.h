/*
 * MultiJump.h
 *
 *  Created on: 24.11.2020
 *      Author: julius
 */

#ifndef SWONE_GAME_OBJECT_EXTENSIONS_MULTIJUMP_H
#define SWONE_GAME_OBJECT_EXTENSIONS_MULTIJUMP_H

#include <string>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include "util/Helper.h"
#include "util/reader/Reader.h"
#include "game/object/Extension.h"

class MultiJump : public Extension {
public:
	static constexpr const char* S_JUMPS_POSSIBLE = "possible jumps";
	static constexpr const char* S_JUMPS = "jumps";

	MultiJump(GameObject* obj, StringMapMap& setupMap);
	void applyConfig(StringMapMap& setupMap) override;
	void getConfig(StringMapMap& extensionMap) override;

	void onTileCollision(ph::Manifold* manifold, Tile* t) override;

	void event(GameObject::Event e) override;
private:
	void jump();
	void resetJump();

	unsigned int m_jumpsPossible = 1; // possible jumps without landing
	unsigned int m_jumps = 0; // count of current jumps

	float m_posVelY = 0;
};

#endif /* SWONE_GAME_OBJECT_EXTENSIONS_MULTIJUMP_H */
