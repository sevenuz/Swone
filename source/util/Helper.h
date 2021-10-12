#ifndef SWONE_UTIL_HELPER_H
#define SWONE_UTIL_HELPER_H

#include <stdlib.h>
#include <vector>
#include <map>
#include <functional>
#include <string>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <tinydir.h>
#include <md5.h>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

#include "physics/IEMath.h"

static constexpr const char* PROJECT_NAME = "swone";

namespace Helper
{
	// return path for saving files
	std::string getSavePath(const std::string& file = "");
	// use xdg-open
	void openLinkOrFile(std::string);
	// return time since epoch (1970?) in microseconds
	long now();
	std::string parseFileName(std::string path);

	void readDirectory(tinydir_dir& dir, std::function<void(tinydir_file& file)> fn, bool recursive = false);
	void readDirectory(std::string path, std::function<void(tinydir_file& file)> fn, bool recursive = false);

	void readFileBytes(std::string file, std::function<void(size_t length, char* data)> fn);
	void writeFileBytes(std::string file, size_t length, const char* data);

	std::string md5(const std::string& filename);

	std::string replaceIllegalCharacters(std::string s, std::string illegalCharacters, char replace);
	void replaceAll(std::string& s, std::string toReplace, std::string replacement);
	std::string trim(const std::string& s, const std::string& whitespace = " \t");

	// static convert fn
	std::vector<std::string> split(std::string str, char splitter);
	// splits by "," and converts with given fn.
	// if length is set, an error is thrown if the tupel has an other length
	template<typename T>
	std::vector<T> toTupel(std::string str, T (*conv)(std::string), size_t length = 0);
	int toInt(std::string s);
	long toLong(std::string s);
	float toFloat(std::string s);
	float toFloat(std::string s, float fallbackValue);
	double toDouble(std::string s);
	sf::Color toColor(std::string s);
	sf::Vector2f toVector2f(std::string s);
	sf::IntRect toIntRect(std::string s);
	sf::FloatRect toFloatRect(std::string s);
	// input should be true or false
	bool toBool(std::string s);

	sf::Vector2f toSfVec(ph::Vec2 v);
	ph::Vec2 toPhVec(sf::Vector2f v);
};

#endif // SWONE_UTIL_HELPER_H
