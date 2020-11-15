#ifndef SWONE_HANDLEABLE_H
#define SWONE_HANDLEABLE_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include "Eventable.h"
#include "Updateable.h"

class Handleable : public sf::Drawable, public sf::Transformable, public Updateable, public Eventable {
public:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	virtual void event(sf::Event& e) = 0;
	virtual void update(sf::Time elapsed) = 0;
	virtual ~Handleable() = 0;
};

inline Handleable::~Handleable() {}

#endif // SWONE_EVENTABLE_H
