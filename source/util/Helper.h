#ifndef SWONE_UTIL_HELPER_H
#define SWONE_UTIL_HELPER_H

#include <vector>
#include <functional>
#include <string>
#include <sstream>
#include <stdexcept>

#include <tinydir.h>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include "physics/IEMath.h"

class Helper
{
public:
	// Not posible to create Objects of this class
	Helper() = delete;
	~Helper() {};
	Helper(Helper const&) = delete;
	void operator=(Helper const&) = delete;

	static void readDirectory(tinydir_dir& dir, std::function<void(tinydir_file& file)> fn);
	static void readDirectory(std::string path, std::function<void(tinydir_file& file)> fn);

	// static convert fn
	static std::vector<std::string> split(std::string str, char splitter);
	// splits by "," and converts with given fn.
	// if length is set, an error is thrown if the tupel has an other length
	template<typename T>
	static std::vector<T> toTupel(std::string str, T (*conv)(std::string), size_t length = 0);
	static int toInt(std::string s);
	static long toLong(std::string s);
	static float toFloat(std::string s);
	static double toDouble(std::string s);
	static sf::Color toColor(std::string s);
	static sf::Vector2f toVector2f(std::string s);
	static sf::IntRect toIntRect(std::string s);
	static sf::FloatRect toFloatRect(std::string s);
	// input should be true or false
	static bool toBool(std::string s);

	static sf::Vector2f toSfVec(ph::Vec2 v);
	static ph::Vec2 toPhVec(sf::Vector2f v);
protected:

private:
};

#endif // SWONE_UTIL_HELPER_H
