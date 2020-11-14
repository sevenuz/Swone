#include "util/reader/Reader.h"

Reader::Reader(std::string path)
{
	m_path = path;
	read();
}

Reader::Reader() {}

Reader::~Reader()
{
	//?
}

void Reader::setPath(std::string path)
{
	m_path = path;
};

void Reader::read()
{
	if (m_path == "")
	{
		throw std::invalid_argument("path not set");
	}
	startReading();
	m_lineCounter = 0;
	std::ifstream infile(m_path);
	if(!infile.is_open())
	{
		throw std::invalid_argument("could not open file: " + m_path);
	}

	std::string line;
	while (std::getline(infile, line))
	{
		// ignore comment lines:
		if (line.substr(0, 2) == "//")
			continue;
		parseLine(m_paragraph, line);
		m_lineCounter++;
	}
	endReading();
};

void Reader::write(std::string file, std::map<std::string, StringMap> m)
{
	std::ofstream outfile(file);
	if (outfile.is_open())
	{
		for(auto p: m){
			outfile << p.first << std::endl;
			for(auto s: p.second){
				outfile << s.first << "=" << s.second << std::endl;
			}
		}
		outfile << std::endl;
		outfile.close();
	}
	else
		Log::ger().log("Could not write file: " + file, Log::Label::Error);
}


void Reader::forEach(std::function<void(std::string, std::string, std::string)> fn) {
	for(auto p: getParagraphMap()){
		for(auto s: p.second){
			fn(p.first, s.first, s.second);
		}
	}
}

std::map<std::string, StringMap>& Reader::getParagraphMap() {
	return m_paragraphMap;
}

StringMap& Reader::getParagraphStringMap(std::string paragraph) {
	return m_paragraphMap[paragraph];
}

size_t Reader::getLineCounter() {
	return m_lineCounter;
}

StringPair Reader::parseValue(std::string line) const {
	size_t next = std::string::npos;
	next = line.find_first_of("=", 0);
	if (next != std::string::npos) {
		return StringPair(line.substr(0, next), line.substr(next + 1));
	}
	else {
		throw std::invalid_argument("line is not a key=value pair");
	}
};

void Reader::startReading() {
	Log::ger().log("start reading");
}

void Reader::endReading() {
	Log::ger().log("end reading");
}

std::string Reader::getParagraph() {
	return m_paragraph;
}

void Reader::setParagraph(std::string p) {
	m_paragraph = p;
}

void Reader::addParagraphValue(std::string paragraph, StringPair p) {
	m_paragraphMap[paragraph].insert(p);
}

void Reader::parseLine(std::string paragraph, std::string line)
{
	try {
		addParagraphValue(paragraph, parseValue(line));
	}
	catch (const std::invalid_argument& ia) {
		setParagraph(line);
	}
};

// convert fn
std::vector<std::string> Reader::split(std::string str, char splitter)
{
	std::stringstream ss(str);
	std::string segment;
	std::vector<std::string> seglist;

	while(std::getline(ss, segment, splitter))
	{
		 seglist.push_back(segment);
	}
	return seglist;
}

int Reader::toInt(std::string s)
{
	return std::stoi(s);
}

long Reader::toLong(std::string s)
{
	return std::stol(s);
}

float Reader::toFloat(std::string s)
{
	return std::stof(s);
}

double Reader::toDouble(std::string s)
{
	return std::stod(s);
}

sf::Color Reader::toColor(std::string s)
{
	if(s.find_first_of("Color(")==std::string::npos || s.find_first_of(")")==s.size())
		throw std::invalid_argument("string not of type Color(r,g,b)");
	std::vector<std::string> values = split(s.substr(6), ',');
	if(values.size()!=3)
		throw std::invalid_argument("Color(r,g,b) needs 3 int values");
	Log::ger().log(values[0]);
	Log::ger().log(values[1]);
	Log::ger().log(values[2]);
	return sf::Color(toInt(values[0]), toInt(values[1]), toInt(values[2]));
}

bool Reader::toBool(std::string s)
{
	if(s=="true")
		return true;
	else if(s=="false")
		return false;
	else
		throw std::invalid_argument("Bool can be \"true\" or \"false\" only");
}

