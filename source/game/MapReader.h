#ifndef MAPREADER_H
#define MAPREADER_H

#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <game/Map.h>
#include <Controller.h>

class MapReader
{
public:
    MapReader(Controller & c, std::string path, Map * mapObj);
    MapReader(Controller & c);
    virtual ~MapReader();
    void setPath(std::string path);
    void setMap(Map * mapObj);
    void readMap();
protected:
private:
    Map * m_map = NULL;
    Controller & m_controller;

    std::string m_path = "";

    bool m_mapParse = false;
    unsigned int m_lineCounter = 0;

    void parseLine(std::string line);
    void parseValue(std::string key, std::string value);
    void parseMap(std::string line);
    void startParseMap();
    void endParseMap();
    void parseMapChar(char c, unsigned int h, unsigned int w);
};

#endif // MAPREADER_H
