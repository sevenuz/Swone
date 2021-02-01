#ifndef SWONE_CLIENT_GAME_INFO_PANEL_H
#define SWONE_CLIENT_GAME_INFO_PANEL_H

#include "imgui.h"
#include "imgui-SFML.h"

#include "client/Controller.h"

class InfoPanel {
public:
	InfoPanel(Controller& c);
	virtual ~InfoPanel();

	void drawImgui();
private:
	Controller& m_controller;
	ClientGameController& m_gc;
};

#endif
