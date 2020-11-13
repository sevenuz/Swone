#include "util/reader/MapReader.h"

MapReader::MapReader(std::string path, Map* mapObj) : Reader(path), m_map(mapObj) {}

MapReader::MapReader() : Reader() {}

MapReader::~MapReader()
{
	// m_map could also have pointer else where
	//delete m_map;
}

void MapReader::setMap(Map* mapObj)
{
	m_map = mapObj;
}

void MapReader::endReading() {
	Reader::endReading();
	m_map->createMapImage();
}

void MapReader::setParagraph(std::string p) {
	parseMap(p);
}

void MapReader::addParagraphValue(std::string paragraph, StringPair p) {
	Log::ger().log("KEY: " + p.first + " VALUE: " + p.second);

	if (p.first == "width")
		m_map->setWidth(std::stoi(p.second));
	else if (p.first == "height")
		m_map->setHeight(std::stoi(p.second));
	else if (p.first == "border")
		m_map->setBorder(MapReader::charToMapTile(p.second[0]));
	else if (p.first == "name")
		m_map->setName(p.second);
}

void MapReader::parseLine(std::string paragraph, std::string line)
{
	if (m_mapParse)
	{
		parseMap(line);
		return;
	}
	Reader::parseLine(paragraph, line);
};

void MapReader::parseMap(std::string line)
{
	if (line == MAP_START_TOKEN)
	{
		startParseMap();
	}
	else if (line == MAP_END_TOKEN)
	{
		endParseMap();
	}
	else if (m_mapParse)
	{
		if (!(m_mapLineCounter < m_map->getHeight()))
		{
			endParseMap();
			return;
		}
		for (size_t i = 0; i < m_map->getWidth(); i++)
		{
			if (i < line.length())
			{
				parseMapChar(line[i], m_mapLineCounter, i);
			}
			else
			{
				//if line is not defined for the full width it becomes whitespace
				parseMapChar(*" ", m_mapLineCounter, i);
			}

		}
		Log::ger().log(line + " " + std::to_string(m_mapLineCounter));
		m_mapLineCounter++;
	}
};

void MapReader::startParseMap()
{
	m_mapParse = true;
	m_mapLineCounter = 0;
	m_map->iniMapData();
	Log::ger().log("start map parsing");
}

void MapReader::endParseMap() {
	//if linecount is not defined for the full height it becomes whitespace
	for (size_t j = m_mapLineCounter; j < m_map->getHeight(); j++) {
		for (size_t i = 0; i < m_map->getWidth(); i++) {
			parseMapChar(*" ", m_mapLineCounter, i);
		}
		Log::ger().log(std::to_string(m_mapLineCounter));
		m_mapLineCounter++;
	}
	m_mapParse = false;
	Log::ger().log("end of map parsing");
}

MapTile MapReader::charToMapTile(char c)
{
	switch (c)
	{
	case '#':
		return MapTile::SHARP;
	case '_':
		return MapTile::UNDERSCORE;
	case ' ':
		return MapTile::SPACE;
	case 'w':
		return MapTile::W;
	default:
		return MapTile::DEFAULT;
	}
}

void MapReader::parseMapChar(char c, size_t h, size_t w)
{
	m_map->setMapDataValue(h, w, MapReader::charToMapTile(c));
}

