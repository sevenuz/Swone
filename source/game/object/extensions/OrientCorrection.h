/*
 * OrientCorrection.h
 *
 *  Created on: 14.12.2020
 *      Author: julius
 */

#ifndef SWONE_GAME_OBJECT_EXTENSIONS_ORIENTCORRECTION_H
#define SWONE_GAME_OBJECT_EXTENSIONS_ORIENTCORRECTION_H

#include "game/object/Extension.h"

class OrientCorrection : public Extension {
public:
	OrientCorrection(GameObject* obj);
	void update(sf::Time ellapsed) override;
	void onTileCollision(ph::Manifold* manifold, Tile* t) override;
private:
	bool checkOrient();

	bool m_correctOrient = false;
	float m_correction = 0.8;
};

#endif /* SWONE_GAME_OBJECT_EXTENSIONS_MULTIJUMP_H */
