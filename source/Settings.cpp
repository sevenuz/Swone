#include <Settings.h>

Settings::Settings()
{
	if (!font.loadFromFile(fontSrc)) {
		//throw std::invalid_argument("font not found");
	}
}

Settings::~Settings()
{
	//dtor
}


int Settings::toW(int w) {
	return (int)(((float)w / STANDART_WIDTH) * WIDTH);
}
int Settings::toH(int h) {
	return (int)(((float)h / STANDART_HEIGHT) * HEIGHT);
}
int Settings::toW(float w) {
	return (int)(w * WIDTH);
}
int Settings::toH(float h) {
	return (int)(h * HEIGHT);
}
int Settings::toF(int f) {
	return (int)(((float)f / STANDART_WIDTH) * WIDTH);
	/*
	if (WIDTH > 800) {
		return f + 30;
	} else {
		return f;
	}
	*/
}
