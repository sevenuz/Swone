#ifndef SWONE_UTIL_READER_READER_H
#define SWONE_UTIL_READER_READER_H

#include <functional>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <utility>

#include "util/Helper.h"

typedef std::map<std::string, std::string> StringMap;
typedef std::map<std::string, StringMap> StringMapMap;
typedef std::pair<std::string, std::string> StringPair;

/*
 * This class reads files containing key value pairs with following scheme:
 * key=value
 * whitespaces are not trimmed so they remain
 * behind or in front of keys and values.
 * Comments can be made with "//"
 * key value pairs can be structured in paragraphs. The default paragraph
 * is called "global". Change to another paragraph by typing the name in a line
 * without a "=" in the name.
 *
 */

class Reader {
public:
	// https://stackoverflow.com/a/24341128
	static constexpr const char* DEFAULT_PARAGRAPH = "global";

	Reader(std::string path);
	Reader();
	virtual ~Reader();

	void setPath(std::string path);
	void read();
	static void write(std::string file, StringMapMap);

	// fn is callback with two parameters:
	// void fn(std::string paragraph, std::string key, std::string value);
	void forEach(std::function<void(std::string, std::string, std::string)> fn);

	StringMapMap& getParagraphMap();
	StringMapMap copyParagraphMap();
	StringMap& getParagraphStringMap(std::string paragraph);

	size_t getLineCounter();

	StringPair parseValue(std::string line) const;

	// is called before the first line of the file is processed
	virtual void startReading();
	// is called after the last line of the file is processed
	virtual void endReading();

	std::string getParagraph();
	// called if line is not a key value pair
	// sets the new paragraph
	virtual void setParagraph(std::string p);
	// called if line is a key value pair
	// adds the key value pair in current paragraph map
	virtual void addParagraphValue(std::string paragraph, StringPair pair);

	// called with every line from the input file
	// calls addParagraphValue or setParagraph
	// if you want to implement complete own logic use this
	virtual void parseLine(std::string paragraph, std::string line);
protected:
private:
	std::string m_path = "";

	size_t m_lineCounter = 0;

	std::string m_paragraph = DEFAULT_PARAGRAPH;
	StringMapMap m_paragraphMap;
};

#endif // SWONE_UTIL_READER_READER_H
