#ifndef SWONE_CLIENT_GAME_GAMEWINDOW_H
#define SWONE_CLIENT_GAME_GAMEWINDOW_H

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/Drawable.hpp>
#include "Handleable.h"
#include "client/Controller.h"

class GameWindow : public Handleable {
public:
	enum GameState : char { Play, Pause };

	GameWindow(Controller& c);
	virtual ~GameWindow();

	void setViewZoom();
	void setViewCenter(sf::Vector2f pos);
	sf::Vector2f getPlayerCenter();
	sf::View getView();

	void drawImgui();
	void drawCharacterSelection();
	void drawInfoPanel();
	void drawPause();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	Controller& m_c;
	GameController& m_gc;
	sf::View m_view;
	sf::Vector2f m_viewDelta = sf::Vector2f(0, 0);

	GameState m_gstate = GameState::Play;
	bool m_showCharacterSelection = true;
	bool m_showInfoPanel = false;
};

#endif
