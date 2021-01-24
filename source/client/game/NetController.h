#ifndef SWONE_CLIENT_GAME_NET_CONTROLLER_H
#define SWONE_CLIENT_GAME_NET_CONTROLLER_H

#include "game/GameController.h"
#include "game/Net.h"

class NetController {
public:
	NetController(Net::Port p, GameController& gc);
	virtual ~NetController();
private:
	GameController& m_gc;
};

#endif
