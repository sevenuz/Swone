#include "Helper.h"

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
		throw std::invalid_argument("Tupel needs format (a,b,c,...)" + str);
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

bool Helper::toBool(std::string s)
{
	if(s=="true")
		return true;
	else if(s=="false")
		return false;
	else
		throw std::invalid_argument("Bool can be \"true\" or \"false\" only");
}

