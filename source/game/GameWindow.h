#ifndef SWONE_GAME_GAMEWINDOW_H
#define SWONE_GAME_GAMEWINDOW_H

#include <SFML/Graphics/Drawable.hpp>
#include <Handleable.h>
#include <game/GameController.h>

class GameWindow: public sf::Drawable, public Handleable {

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	GameController & m_gc;

protected:

public:
	GameWindow(GameController& gc);
	virtual ~GameWindow();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);

};

#endif
