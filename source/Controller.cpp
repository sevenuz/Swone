#include <Controller.h>

Controller::Controller(sf::RenderWindow& w) : m_window(w) {
	iniSettings();
}

Controller::~Controller() {
	Log::ger().log("Controller destroyed");
}

void Controller::iniSettings() {
}

void Controller::setActiveWindow(ActiveWindow i) {
	if (i > ActiveWindow::AW_LAST) {
		m_activeWindow = ActiveWindow::AW_FIRST;
	}
	else {
		m_activeWindow = i;
	}
}
ActiveWindow Controller::getActiveWindow() {
	return m_activeWindow;
}

void Controller::setActiveGameWindow(ActiveGameWindow i) {
	if (i > ActiveGameWindow::AGW_LAST) {
		m_activeGameWindow = ActiveGameWindow::AGW_FIRST;
	}
	else {
		m_activeGameWindow = i;
	}
}
ActiveGameWindow Controller::getActiveGameWindow() {
	return m_activeGameWindow;
}

sf::Vector2f Controller::getScale() {
	return m_scale;
}

void Controller::setScale(sf::Vector2f scale) {
	m_scale = scale;
}


sf::RenderWindow& Controller::getWindow() {
	return m_window;
}

void Controller::setView(sf::View view) {
	getWindow().setView(view);
}

void Controller::setDefaultView() {
	getWindow().setView(getWindow().getDefaultView());
}
