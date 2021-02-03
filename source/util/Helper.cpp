#include "Helper.h"

std::string Helper::getSavePath(const std::string& s)
{
	std::filesystem::path p = s;
	if(p.is_relative()) {
		bool isEmpty = true;
#if defined _WIN64 || defined _WIN32
		p = getenv("APPDATA");
		isEmpty = path.empty();
		p += "/" + PROJECT_NAME + "/";
#elif __APPLE__
		p = getenv("HOME");
		isEmpty = path.empty();
		p += "/" + PROJECT_NAME + "/";
#elif __linux
		p = getenv("HOME");
		isEmpty = p.empty();
		p += "/." + std::string(PROJECT_NAME) + "/";
#endif
		if(isEmpty)
			throw "No Directory to save Files could be found!";
		p += s;
	}
	return p;
}

void Helper::openLinkOrFile(std::string s) // TODO for windows and ios
{
#if defined _WIN64 || defined _WIN32
#elif __APPLE__
#elif __linux
	if (system(NULL)) {
		s = "xdg-open " + s;
		system(s.c_str());
	} else {
		Log::ger().log("Could not open, no command processor found.", Log::Label::Error);
	}
#endif
}

long Helper::now()
{
	auto now = std::chrono::system_clock::now().time_since_epoch();
	long time = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
	return time;
}

std::string Helper::parseFileName(std::string path)
{
	// https://www.cplusplus.com/reference/string/string/find_last_of/
	// parse filename, also works on window
	std::size_t found = path.find_last_of("/\\");
	return path.substr(found+1);
}

void Helper::readDirectory(tinydir_dir& dir, std::function<void(tinydir_file& file)>fn, bool recursive)
{
	if (!dir.has_next) {
		throw std::invalid_argument("no files or no dir available");
	}

	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);

		if (recursive && file.is_dir) {
			if (strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0) {
				tinydir_open(&dir, file.path);
				readDirectory(dir, fn, recursive);
			}
		} else {
			fn(file);
		}

		tinydir_next(&dir);
	}
}

void Helper::readDirectory(std::string path, std::function<void(tinydir_file& file)>fn, bool recursive)
{
	tinydir_dir dir;
	tinydir_open(&dir, path.c_str());
	readDirectory(dir, fn, recursive);
	tinydir_close(&dir);
}

void Helper::readFileBytes(std::string file, std::function<void(size_t length, char* data)> fn)
{
	// http://www.codecodex.com/wiki/Read_a_file_into_a_byte_array#C.2B.2B
	std::ifstream fl(file, std::ios::binary);
	fl.seekg(0, std::ios::end);
	size_t len = fl.tellg();
	char* dat = new char[len];
	fl.seekg(0, std::ios::beg);
	fl.read(dat, len);
	fn(len, dat);
	delete[] dat;
	fl.close();
}

void Helper::writeFileBytes(std::string file, size_t length, const char* data)
{
	std::ofstream(file, std::ios::binary).write(data, length);
}

std::string Helper::replaceIllegalCharacters(std::string s, std::string ic, char r)
{
	for(auto it = s.begin(); it < s.end(); ++it){
		if(ic.find(*it) != std::string::npos){
			*it = r;
		}
	}
	return s;
}

void Helper::replaceAll(std::string& s, std::string toReplace, std::string replacement)
{
	size_t len = toReplace.size();
	size_t pos = s.find(toReplace);
	while(pos != std::string::npos) {
		s.replace(pos, len, replacement);
		pos = s.find(toReplace);
	}
}

// https://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string
std::string Helper::trim(const std::string& str, const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
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
