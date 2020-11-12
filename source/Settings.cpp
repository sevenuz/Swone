#include <Settings.h>

Settings::Settings()
{
	Reader r(SETTINGS_FILE);
	r.forEach([](std::string p, StringPair s){
		std::cout << p << " " << s.first << "=" << s.second << std::endl;
	});
	if (!font.loadFromFile(fontSrc)) {
		//throw std::invalid_argument("font not found");
	}
}

Settings::~Settings()
{
	//dtor
}

int Settings::toW(float w){
	return (int)(w*STANDARD_WIDTH);
}

int Settings::toH(float h){
	return (int)(h*STANDARD_HEIGHT);
}

int Settings::toW(int w){
	return toW(((float)w/STANDARD_WIDTH));
}

int Settings::toH(int h){
	return toH(((float)h/STANDARD_HEIGHT));
}

int Settings::toF(int f){
	return toW(f);
}

