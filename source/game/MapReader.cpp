#include <game/MapReader.h>

MapReader::MapReader(Controller & c,std::string path, Map * mapObj):m_map(mapObj), m_controller(c)
{
    m_path = path;
    readMap();
}

MapReader::MapReader(Controller & c): m_controller(c) {
}

MapReader::~MapReader()
{
    //dtor
}

void MapReader::setPath(std::string path)
{
    m_path = path;
};

void MapReader::setMap(Map * mapObj)
{
    m_map = mapObj;
}

void MapReader::readMap()
{
    if(m_path == "" || m_map == NULL)
    {
        throw std::invalid_argument("path or map not set");
    }
    m_controller.pushLogMsg("start reading map" + m_path);
    m_lineCounter = 0;
    std::ifstream infile(m_path);
    std::string line;
    while (std::getline(infile, line))
    {
        parseLine(line);
    }
    m_controller.pushLogMsg("end reading map.");
    m_map->createMapImage();
};

void MapReader::parseLine(std::string line)
{
    if(m_mapParse)
    {
        parseMap(line);
        return;
    }
    size_t next = -1;
    next = line.find_first_of( "=", 0 );
    if(next != std::string::npos)
    {
        parseValue(line.substr( 0, next ), line.substr( next + 1 ));
    }
    else
    {
        parseMap(line);
    }
};

void MapReader::parseValue(std::string key, std::string value)
{
    m_controller.pushLogMsg("KEY: " + key + " VALUE: " + value);
    if(key == "width")
    {
        m_map->setWidth(std::stoi(value));
    }
    else if(key == "height")
    {
        m_map->setHeight(std::stoi(value));
    }
    else if(key == "border")
    {
        m_map->setBorder(value);
    }
    else if(key == "name")
    {
    	m_map->setName(value);
    }
};

void MapReader::parseMap(std::string line)
{
    if(line == "map:start")
    {
        startParseMap();
    }
    else if(line == "map:end")
    {
        endParseMap();
    }
    else if(m_mapParse)
    {
        if(!(m_lineCounter < m_map->getHeight()))
        {
            endParseMap();
            return;
        }
        for(unsigned int i = 0; i < m_map->getWidth(); i++)
        {
            if(i < line.length())
            {
                parseMapChar(line[i], m_lineCounter, i);
            }
            else
            {
                //if line is not defined for the full width it becomes whitespace
                parseMapChar(*" ", m_lineCounter, i);
            }

        }
        m_controller.pushLogMsg(line + " " + std::to_string(m_lineCounter));
        m_lineCounter++;
    }
};

void MapReader::startParseMap()
{
    m_mapParse = true;
    m_map->iniMapData();
    m_controller.pushLogMsg("start map parsing");
}

void MapReader::endParseMap() {
	//if linecount is not defined for the full height it becomes whitespace
	for(unsigned int j = m_lineCounter; j < m_map->getHeight(); j++) {
		for(unsigned int i = 0; i < m_map->getWidth(); i++) {
		        parseMapChar(*" ", m_lineCounter, i);
		}
		m_controller.pushLogMsg(std::to_string(m_lineCounter));
		m_lineCounter++;
	}
   	m_mapParse = false;
    	m_controller.pushLogMsg("end of map parsing");
}

void MapReader::parseMapChar(char c, unsigned int h, unsigned int w)
{
    switch(c)
    {
    case *"#":
        m_map->setMapDataValue(h, w, MapTiles::SHARP);
        break;
    case *"_":
        m_map->setMapDataValue(h, w, MapTiles::UNDERSCORE);
        break;
    case *" ":
        m_map->setMapDataValue(h, w, MapTiles::SPACE);
        break;
    case *"w":
        m_map->setMapDataValue(h, w, MapTiles::W);
        break;
    default:
        m_map->setMapDataValue(h, w, MapTiles::DEFAULT);
    }
}
