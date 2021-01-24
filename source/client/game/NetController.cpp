#include <client/game/NetController.h>

NetController::NetController(Net::Port p, GameController& gc) :
	m_gc(gc) 
{}

NetController::~NetController()
{}

