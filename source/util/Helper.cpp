#include "Helper.h"

long Helper::now()
{
	auto now = std::chrono::system_clock::now().time_since_epoch();
	long time = std::chrono::duration_cast<std::chrono::seconds>(now).count();
	return time;
}

std::string Helper::parseFileName(std::string path)
{
	// https://www.cplusplus.com/reference/string/string/find_last_of/
	// parse filename, also works on window
	std::size_t found = path.find_last_of("/\\");
	return path.substr(found+1);
}

void Helper::readDirectory(tinydir_dir& dir, std::function<void(tinydir_file& file)>fn)
{
	if (!dir.has_next) {
		throw std::invalid_argument("no files or no dir available");
	}

	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);

		fn(file);

		tinydir_next(&dir);
	}
}

void Helper::readDirectory(std::string path, std::function<void(tinydir_file& file)>fn)
{
	tinydir_dir dir;
	tinydir_open(&dir, path.c_str());
	readDirectory(dir, fn);
	tinydir_close(&dir);
}

// convert fn
std::vector<std::string> Helper::split(std::string str, char splitter)
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

template<typename T>
std::vector<T> Helper::toTupel(std::string str, T (*conv)(std::string), size_t length)
{
	if(str[0]!='(' && str[str.length()-1]!=')')
		throw std::invalid_argument("Tupel needs format (a,b,c,...) but is " + str);
	// replace bracets
	str = str.replace(0,1,"").replace(str.length()-1,1,"");
	std::vector<std::string> v = split(str, ',');
	if(length>0 && v.size()!=length)
		throw std::invalid_argument("Tupel violates length");
	std::vector<T> values;
	for(std::string s : v)
		values.push_back(conv(s));
	return values;
}

int Helper::toInt(std::string s)
{
	return std::stoi(s);
}

long Helper::toLong(std::string s)
{
	return std::stol(s);
}

float Helper::toFloat(std::string s)
{
	return std::stof(s);
}

float Helper::toFloat(std::string s, float fallbackValue)
{
	try {
		return Helper::toFloat(s);
	} catch(...) {
		return fallbackValue;
	}
}


double Helper::toDouble(std::string s)
{
	return std::stod(s);
}

sf::Color Helper::toColor(std::string s)
{
	if(s.compare("Color")==-1)
		throw std::invalid_argument("string not of type Color(r,g,b)");
	std::vector<int> values = toTupel(s.substr(5), toInt, 3);
	return sf::Color(values[0], values[1], values[2]);
}

sf::Vector2f Helper::toVector2f(std::string s)
{
	std::vector<float> values = toTupel(s, toFloat, 2);
	return sf::Vector2f(values[0], values[1]);
}

sf::IntRect Helper::toIntRect(std::string s)
{
	std::vector<int> values = toTupel(s, toInt, 4);
	return sf::IntRect(values[0], values[1], values[2], values[3]);
}

sf::FloatRect Helper::toFloatRect(std::string s)
{
	std::vector<float> values = toTupel(s, toFloat, 4);
	return sf::FloatRect(values[0], values[1], values[2], values[3]);
}

bool Helper::toBool(std::string s)
{
	if(s=="true")
		return true;
	else
		return false;
}

// TODO Expression Template (convert at compile time)
sf::Vector2f Helper::toSfVec(ph::Vec2 v)
{
	return sf::Vector2f(v.x, v.y);
}
// TODO Expression Template (convert at compile time)
ph::Vec2 Helper::toPhVec(sf::Vector2f v)
{
	return ph::Vec2(v.x, v.y);
}

const sf::Texture* Helper::loadTexture(std::string path)
{
	if(Helper::getTextureMap().count(path)){
		return Helper::getTextureMap()[path];
	} else {
		sf::Texture* t = new sf::Texture();
		if (!t->loadFromFile(path)) {
			Log::ger().log(path + ": Failed to load texture", Log::Label::Error);
			throw std::invalid_argument("Failed to load texture");
		}
		Helper::getTextureMap()[path] = t;
		return t;
	}
}

const sf::Image* Helper::loadImage(std::string path)
{
	if(Helper::getImageMap().count(path)){
		return Helper::getImageMap()[path];
	} else {
		sf::Image* t = new sf::Image();
		if (!t->loadFromFile(path)) {
			Log::ger().log(path + ": Failed to load texture", Log::Label::Error);
			throw std::invalid_argument("Failed to load texture");
		}
		Helper::getImageMap()[path] = t;
		return t;
	}
}
