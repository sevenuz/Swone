#ifndef SWONE_UTIL_READER_MAPREADER_H
#define SWONE_UTIL_READER_MAPREADER_H

#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "util/reader/Reader.h"
#include "game/Map.h"
#include "Controller.h"

class MapReader: public Reader
{
public:
    const std::string MAP_START_TOKEN = "map:start";
    const std::string MAP_END_TOKEN = "map:end";

    MapReader(Controller & c, std::string path, Map * mapObj);
    MapReader(Controller & c);
    virtual ~MapReader();

    void setMap(Map * mapObj);
    static MapTile charToMapTile(char);
protected:
private:
    Map * m_map = NULL;

    bool m_mapParse = false;
    size_t m_mapLineCounter = 0;

    void endReading() override;
    void setParagraph(std::string p) override;
    void addParagraphValue(std::string paragraph, StringPair pair) override;
    void parseLine(std::string paragraph, std::string line) override;

    void parseMap(std::string line);
    void startParseMap();
    void endParseMap();
    void parseMapChar(char c, size_t h, size_t w);
};

#endif // SWONE_UTIL_READER_MAPREADER_H
