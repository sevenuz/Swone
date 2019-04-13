#ifndef SWONE_GAME_GAMECONTROLLER_H
#define SWONE_GAME_GAMECONTROLLER_H

#include <vector>
#include <game/Map.h>
#include <game/Player.h>
#include <Controller.h>

class GameController {
public:
    GameController(Controller & c);
    virtual ~GameController();
    Controller& getController();

    Map * getMap();
    std::vector<Player *> getPlayers();
    void setMap(Map * m);
    void pushPlayer(Player *);
protected:
private:
    Controller & m_controller;
    Map * m_map = NULL;
    std::vector<Player *> m_player;
};
#endif
