#include <client/Controller.h>

Controller::Controller(Settings& settings, sf::RenderWindow& w) : m_settings(settings), m_window(w) {
	m_window.setVerticalSyncEnabled(m_settings.isVerticalSyncEnabled());
}

Controller::~Controller() {}

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

Settings& Controller::getSettings() {
	return m_settings;
}

sf::RenderWindow& Controller::getWindow() {
	return m_window;
}

void Controller::setDefaultView() {
	getWindow().setView(getWindow().getDefaultView());
}

bool Controller::isRunning()
{
	return m_run;
}

void Controller::start()
{
	m_run = true;
}

void Controller::stop()
{
	m_run = false;
}

