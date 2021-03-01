#ifndef SWONE_CLIENT_GAME_CHARACTER_SELECTION_H
#define SWONE_CLIENT_GAME_CHARACTER_SELECTION_H

#include "imgui.h"
#include "imgui-SFML.h"

#include "game/GameReader.h"
#include "util/Helper.h"
#include "client/Controller.h"
#include "client/game/ClientGameController.h"
#include "client/game/NetController.h"

#define MAX_LOCAL_PLAYERS 4
#define PLAYER_NAME_LENGTH 11

class CharacterSelection {
public:
	CharacterSelection(Controller& c);
	virtual ~CharacterSelection();

	void drawImgui();
	void reset();
private:
	Controller& m_c;
	ClientGameController& m_gc;
	NetController& m_nc;

	struct Character {
		std::string selectedCharacter;
		char playerName[PLAYER_NAME_LENGTH];
		ImVec4 playerColor = ImColor(114, 144, 154, 200);
		int selectedKeybindingProfile = 0;
		GameObject* player = nullptr;
	};
	std::vector<Character> m_characters;

	void drawImguiCharacterMenu(size_t index);
	bool verifyCharacter(const Character& c);
};

#endif
