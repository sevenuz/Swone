#ifndef SWONE_GAMEPANEL_H
#define SWONE_GAMEPANEL_H

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
#include "game/GameWindow.h"
#include "Handleable.h"
#include "Settings.h"
#include "Controller.h"
#include "util/reader/MapReader.h"
#include "util/Log.h"


class GamePanel : public Handleable {
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

	void readMapsFromDir();
	void setMapSelection(int);
	void setActionSelection(char);

	void startGame();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
	void update(sf::Time ellapsed);
	void event(sf::Event& e);
	GamePanel(Controller& c);
	~GamePanel();

	detailMap getDetails();
};

#endif
