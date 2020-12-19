#ifndef SWONE_CLIENT_GAMEMENU_H
#define SWONE_CLIENT_GAMEMENU_H

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <string>
#include <sstream>
#include <tinydir.h>

#include "graphics/ParticleSystem.h"
#include "game/GameController.h"
#include "Handleable.h"
#include "client/GameWindow.h"
#include "client/Settings.h"
#include "client/Controller.h"
#include "util/reader/MapReader.h"
#include "util/Log.h"
#include "util/Helper.h"

class GameMenu : public Handleable {
private:
	enum GamePanelMenuPoint : char { PLAY, FIRST = PLAY, LAST = PLAY };

	ParticleSystem m_ps;
	Controller& m_controller;
	GameController m_gameController;
	GameWindow m_gameWindow;

	sf::Text m_play;

	sf::Text m_mapName;
	sf::Text m_switchLeft;
	sf::Text m_switchRight;

	MapReader m_mapReader;

	bool m_mapsFound = false;
	// GamePanelMenuPoint type, but char to perform de/increment
	char m_selectedAction = FIRST;
	int m_selectedMap = 0;
	std::vector<Map*> m_maps;

	struct GameObjectSelection {
		GameObject* obj;
		bool selected;
	};
	std::vector<GameObjectSelection> m_gameObjects;
	std::vector<GameObjectSelection> m_gamePlayers;

	void readMapsFromDir();
	void readGameObjectsFromDir();
	void setMapSelection(int);
	void setActionSelection(char);

	void startGame();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
	void drawImgui();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);
	GameMenu(Controller& c);
	~GameMenu();

	const GameController& getGameController() const;
	GameObject* getGameObjectById(const std::string& id) const;

	void updateLog() const;
};

#endif
