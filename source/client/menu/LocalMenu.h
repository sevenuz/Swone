#ifndef SWONE_CLIENT_LOCALMENU_H
#define SWONE_CLIENT_LOCALMENU_H

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Color.hpp>

#include <map>
#include <vector>
#include <string>
#include <sstream>

#include <tinydir.h>

#include "graphics/ParticleSystem.h"
#include "game/GameController.h"
#include "game/GameReader.h"
#include "Handleable.h"
#include "client/Settings.h"
#include "client/Controller.h"
#include "util/Log.h"
#include "util/Helper.h"

class LocalMenu : public Handleable {
private:
	enum GamePanelMenuPoint : char { PLAY, FIRST = PLAY, LAST = PLAY };

	ParticleSystem m_ps;
	Controller& m_controller;

	sf::Text m_play;

	sf::Text m_sceneryName;
	sf::Text m_switchLeft;
	sf::Text m_switchRight;

	// GamePanelMenuPoint type, but char to perform de/increment
	char m_selectedAction = FIRST;
	int m_selectedScenery = 0;
	bool m_sceneriesFound = false;

	struct GameObjectSelection {
		std::string key;
		std::string name;
		bool selected;
	};
	std::vector<Scenery*> m_sceneries;
	std::map<int, std::vector<GameObjectSelection>> m_gamePlayers; // key: m_selectedScenery

	void readSceneries(std::string resDir);

	void setScenerySelection(int);
	void setActionSelection(char);

	void startGame();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
	void drawImgui();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);
	LocalMenu(Controller& c);
	~LocalMenu();

	const GameController& getGameController() const;
	GameObject* getGameObjectById(const std::string& id) const;

	void updateLog() const;
};

#endif
