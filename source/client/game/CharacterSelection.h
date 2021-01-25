#ifndef SWONE_CLIENT_GAME_CHARACTER_SELECTION_H
#define SWONE_CLIENT_GAME_CHARACTER_SELECTION_H

#include "imgui.h"
#include "imgui-SFML.h"

#include "game/GameReader.h"
#include "util/Helper.h"
#include "game/GameController.h"
#include "client/Controller.h"

#define PLAYER_NAME_LENGTH 11

class CharacterSelection {
public:
	CharacterSelection(Controller& c);
	virtual ~CharacterSelection();

	void drawImgui();
private:
	Controller& m_c;
	GameController& m_gc;

	std::string m_selectedPlayer;
	char m_playerName[PLAYER_NAME_LENGTH];
	ImVec4 m_playerColor = ImColor(114, 144, 154, 200);
	int m_selectedControlProfile = 0;
};

#endif
