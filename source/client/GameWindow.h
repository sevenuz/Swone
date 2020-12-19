#ifndef SWONE_CLIENT_GAMEWINDOW_H
#define SWONE_CLIENT_GAMEWINDOW_H

#include <SFML/Graphics/Drawable.hpp>
#include "Handleable.h"
#include "game/GameController.h"
#include "Controller.h"

class GameWindow : public Handleable {
public:
	GameWindow(Controller& c, GameController& gc);
	virtual ~GameWindow();

	void setViewZoom();
	void setViewCenter(sf::Vector2f pos);
	sf::Vector2f getPlayerCenter();
	sf::View getView();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	Controller& m_controller;
	GameController& m_gc;
	sf::View m_view;
	sf::Vector2f m_viewDelta = sf::Vector2f(0, 0);
};

#endif
