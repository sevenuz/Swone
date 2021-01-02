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
		// TODO ignore empty lines
		// ignore comment lines:
		if (line.substr(0, 2) == "//")
			continue;
		parseLine(m_paragraph, line);
		m_lineCounter++;
	}
	endReading();
};

void Reader::write(std::string file, StringMapMap m)
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
		throw std::invalid_argument("Could not write file: " + file);
}


void Reader::forEach(std::function<void(std::string, std::string, std::string)> fn) {
	for(auto p: getParagraphMap()){
		for(auto s: p.second){
			fn(p.first, s.first, s.second);
		}
	}
}

StringMapMap& Reader::getParagraphMap() {
	return m_paragraphMap;
}

StringMapMap Reader::copyParagraphMap() {
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

void Reader::startReading() {}

void Reader::endReading() {}

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
