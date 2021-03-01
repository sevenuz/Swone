#ifndef SWONE_CLIENT_GAME_GAMEWINDOW_H
#define SWONE_CLIENT_GAME_GAMEWINDOW_H

#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include "Handleable.h"
#include "client/Controller.h"
#include "client/game/CharacterSelection.h"
#include "client/game/InfoPanel.h"
#include "game/Net.h"

class GameWindow : public Handleable {
public:
	enum GameState : char { Play, Pause };

	GameWindow(Controller& c);
	virtual ~GameWindow();

	void setViewZoom();
	void setViewCenter(sf::Vector2f pos);
	sf::Vector2f getPlayerCenter() const;
	sf::View getView();

	void drawImgui();
	void drawPause();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);
private:
	bool isEqualEvent(const GameObject::Event& event1, const GameObject::Event& event2);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	Controller& m_c;
	ClientGameController& m_gc;
	NetController& m_nc;

	sf::View m_view;
	sf::Vector2f m_viewDelta = sf::Vector2f(0, 0);

	GameState m_gstate = GameState::Play;
	bool m_showCharacterSelection = true;
	bool m_showInfoPanel = false;
	bool m_showHitboxes = false;

	CharacterSelection m_characterSelection;
	InfoPanel  m_infoPanel;
};

#endif
