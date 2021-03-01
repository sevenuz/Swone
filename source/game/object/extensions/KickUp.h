#ifndef SWONE_GAME_OBJECT_EXTENSIONS_KICKUP_H
#define SWONE_GAME_OBJECT_EXTENSIONS_KICKUP_H

#include <string>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include "util/Helper.h"
#include "util/reader/Reader.h"
#include "game/object/Extension.h"

#define KICK_VEL 15

class KickUp : public Extension {
public:
	KickUp(GameObject* obj);

	void onObjectCollision(ph::Manifold* manifold, GameObject* t) override;

	void event(GameObject::Event e) override;
private:
	bool m_kick = false;
};

#endif /* SWONE_GAME_OBJECT_EXTENSIONS_MULTIJUMP_H */
