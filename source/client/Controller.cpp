#include <client/Controller.h>

Controller::Controller(Settings& settings, sf::RenderWindow& w) :
	m_settings(settings),
	m_window(w)
{
	m_window.setVerticalSyncEnabled(m_settings.isVerticalSyncEnabled());
}

Controller::~Controller() {}

void Controller::pushState(State s)
{
	m_stateStack.push(s);
}

void Controller::popState()
{
	m_stateStack.pop();
}

Controller::State Controller::getState()
{
	if(m_stateStack.empty())
		return State::MainMenu;
	else
		return m_stateStack.top();
}

Settings& Controller::getSettings() {
	return m_settings;
}

sf::RenderWindow& Controller::getWindow() {
	return m_window;
}

GameController& Controller::getGameController()
{
	return m_gc;
}

void Controller::loadGame(Net::JoinLobbyAck jla)
{
	m_gc.loadScenery(m_settings.getResourceDirectory(), jla.fileCheck);
	// TODO NetController
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

