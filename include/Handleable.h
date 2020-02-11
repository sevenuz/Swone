#ifndef SWONE_HANDLEABLE_H
#define SWONE_HANDLEABLE_H

#include <SFML/Window/Event.hpp>
#include <Eventable.h>
#include <Updateable.h>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

class Handleable: public sf::Transformable, public Updateable, public Eventable {//public sf::Drawable,


public:
    //virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {std::cout <<"what??"<<std::endl;};
    void update(sf::Time ellapsed){};
    void event(sf::Event& e){};
    void draw(sf::RenderTarget& target, sf::RenderStates states){};
};

#endif // SWONE_EVENTABLE_H
