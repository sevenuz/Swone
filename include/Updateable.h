#ifndef SWONE_UPDATEABLE_H
#define SWONE_UPDATEABLE_H

#include <SFML/System/Time.hpp>

class Updateable {
public:
    virtual void update(sf::Time elapsed) = 0;
};
#endif
