#ifndef SWONE_EVENTABLE_H
#define SWONE_EVENTABLE_H

#include <SFML/Window/Event.hpp>

class Eventable {
public:
	virtual void event(sf::Event& e) = 0;
	virtual ~Eventable() = 0;
};

inline Eventable::~Eventable() {}

#endif // SWONE_EVENTABLE_H
