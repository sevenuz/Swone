#ifndef SWONE_EVENTABLE_H
#define SWONE_EVENTABLE_H


#include <SFML/Window/Event.hpp>

class Eventable {
public:
    //const damit es nur f�r die subklasse �berschrieben wird und nicht f�r alle die es erben?
    virtual void event(sf::Event& e) = 0;
};

#endif // SWONE_EVENTABLE_H
