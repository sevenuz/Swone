#ifndef SWONE_EVENTABLE_H
#define SWONE_EVENTABLE_H


#include <SFML/Window/Event.hpp>

class Eventable {
public:
    //const damit es nur für die subklasse überschrieben wird und nicht für alle die es erben?
    virtual void event(sf::Event& e) = 0;
};

#endif // SWONE_EVENTABLE_H
