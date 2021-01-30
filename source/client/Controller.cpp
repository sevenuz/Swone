#include <client/Controller.h>

Controller::Controller(Settings& settings, sf::RenderWindow& w) :
	m_settings(settings),
	m_window(w),
	m_nc(*this)
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

NetController& Controller::getNetController()
{
	return m_nc;
}

void Controller::loadGame(Net::JoinLobbyAck jla)
{
	Log::ger().log("loadGame " + jla.code);
	m_gc.loadScenery(m_settings.getResourceDirectory(), jla.fileCheck);
	std::thread(&NetController::start, &m_nc, jla.code, m_settings.getServerIpAddress(), m_settings.getPort()).detach();
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

