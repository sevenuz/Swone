#include <client/menu/LocalMenu.h>

LocalMenu::LocalMenu(Controller& c) :
	m_ps(100),
	m_controller(c),
	m_gameWindow(c, m_gameController)
{
	m_ps.setColor(sf::Color::White);
	m_ps.setDrawingType(sf::Quads);
	m_ps.setLifetime(sf::seconds(3));
	m_ps.setOrigin(m_controller.getSettings().toW(0.4f), m_controller.getSettings().toH(61), 150, 3, Origin::ON_BORDER);

	m_play.setFont(m_controller.getSettings().getFont());
	m_play.setString("Play");
	m_play.setPosition(m_controller.getSettings().toW(0.415f), m_controller.getSettings().toH(0.8f));
	m_play.setFillColor(sf::Color::Yellow);
	m_play.setCharacterSize(m_controller.getSettings().toF(50));

	m_sceneryName.setFont(m_controller.getSettings().getFont());
	m_sceneryName.setPosition(m_controller.getSettings().toW(0.4f), m_controller.getSettings().toH(31));
	m_sceneryName.setFillColor(sf::Color::White);
	m_sceneryName.setCharacterSize(m_controller.getSettings().toF(30));

	m_switchLeft.setFont(m_controller.getSettings().getFont());
	m_switchLeft.setString("<");
	m_switchLeft.setPosition(m_controller.getSettings().toW(30), m_controller.getSettings().toH(150));
	m_switchLeft.setFillColor(sf::Color::Yellow);
	m_switchLeft.setCharacterSize(m_controller.getSettings().toF(50));

	m_switchRight.setFont(m_controller.getSettings().getFont());
	m_switchRight.setString(">");
	m_switchRight.setPosition(m_controller.getSettings().toW(590), m_controller.getSettings().toH(150));
	m_switchRight.setFillColor(sf::Color::Yellow);
	m_switchRight.setCharacterSize(m_controller.getSettings().toF(50));

	readSceneries(m_controller.getSettings().getResourceDirectory());
	m_sceneriesFound = m_sceneries.size() > 0;
	if(m_sceneriesFound)
		setScenerySelection(0);

	for(size_t i = 0; i < m_sceneries.size(); i++) {
		for(auto& p : m_sceneries[i]->getPlayerSetupMaps()) {
			m_gamePlayers[i].push_back({p.first, p.second[Reader::DEFAULT_PARAGRAPH][GameObject::S_NAME], true});
		}
	}
}

LocalMenu::~LocalMenu() {}

void LocalMenu::readSceneries(std::string resDir)
{
	GameReader::readSceneryMaps(resDir);
	for(auto& p : GameReader::getSceneryMaps()) {
		Scenery* s = new Scenery(resDir, Helper::parseFileName(p.first), p.second);
		m_sceneries.push_back(s);
	}
}

void LocalMenu::setScenerySelection(int i) {
	if (!m_sceneriesFound) {
		Log::ger().log("no sceneries found.", Log::Label::Error);
		return;
	}

	if (i >= (int)m_sceneries.size()) {
		i = 0;
	}
	else if (i < 0) {
		i = m_sceneries.size() - 1;
	}

	m_selectedScenery = i;

	m_sceneryName.setString(m_sceneries[m_selectedScenery]->getName());
}

void LocalMenu::setActionSelection(char i) {
	if (i > GamePanelMenuPoint::LAST) {
		i = GamePanelMenuPoint::FIRST;
	}
	else if (i < GamePanelMenuPoint::FIRST) {
		i = GamePanelMenuPoint::LAST;
	}
	m_selectedAction = i;
}

void LocalMenu::startGame() {
	if (m_sceneriesFound) {
		Scenery* s = m_sceneries[m_selectedScenery];
		s->reset();
		m_gameController.setScenery(s);
		m_gameWindow.setViewZoom();
		for(GameObjectSelection& gos : m_gamePlayers[m_selectedScenery])
			if(gos.selected)
				s->spawnPlayer(gos.key);
		m_gameController.startGame();
		m_controller.setActiveGameWindow(ActiveGameWindow::INGAME);
	}
	else {
		Log::ger().log("No Scenery. Can't start the game.", Log::Label::Error);
	}
}

void LocalMenu::event(sf::Event& event) {
	sf::RenderWindow& w = m_controller.getWindow();
	sf::Vector2i pixelPos = sf::Mouse::getPosition(w);
	sf::Vector2f worldPos = w.mapPixelToCoords(pixelPos);

	switch (m_controller.getActiveGameWindow()) {
	case ActiveGameWindow::MAPSELECTION:
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) {
				m_controller.setActiveMenu(ActiveMenu::MAIN);
			}
			if (event.key.code == sf::Keyboard::Left) {
				setScenerySelection(m_selectedScenery - 1);
			}
			if (event.key.code == sf::Keyboard::Right) {
				setScenerySelection(m_selectedScenery + 1);
			}
			if (event.key.code == sf::Keyboard::Down) {
				setActionSelection(m_selectedAction++);
			}
			if (event.key.code == sf::Keyboard::Up) {
				setActionSelection(m_selectedAction--);
			}
			if (event.key.code == sf::Keyboard::P) {
				//Spawn Player
			}
			if (event.key.code == sf::Keyboard::Return) {//Enter
				startGame();
			}
		}
		else if (event.type == sf::Event::MouseButtonReleased) {
			if (m_play.getGlobalBounds().contains(worldPos.x, worldPos.y)) {
				startGame();
			}
			else if (m_switchLeft.getGlobalBounds().contains(worldPos.x, worldPos.y)) {
				setScenerySelection(m_selectedScenery - 1);
			}
			else if (m_switchRight.getGlobalBounds().contains(worldPos.x, worldPos.y)) {
				setScenerySelection(m_selectedScenery + 1);
			}
		}
		break;
	case ActiveGameWindow::INGAME:
		m_gameWindow.event(event);
		break;
	default:
		break;
	}
}

void LocalMenu::update(sf::Time ellapsed) {
	m_ps.update(ellapsed);
	switch (m_controller.getActiveGameWindow()) {
	case ActiveGameWindow::MAPSELECTION:
		//m_sceneries[m_selectedScenery]->getSprite().setScale(sf::Vector2f(0.25, 0.25));
		break;
	case ActiveGameWindow::INGAME:
		m_gameWindow.update(ellapsed);
		break;
	default:
		break;
	}
}

void LocalMenu::drawImgui()
{
	if(m_controller.getActiveGameWindow() != ActiveGameWindow::MAPSELECTION)
		return;

	// imgui.h:595 or imgui_demo.cpp:187
	int window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize |
											ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
	bool* w_open = NULL; // hides close option

	auto player_pos = ImVec2(m_controller.getSettings().toW(0.1f), m_controller.getSettings().toH(0.1f));
	auto window_size = ImVec2(m_controller.getSettings().toW(0.2f), m_controller.getSettings().toH(0.8f));

	std::string no_files_found = "No Files of this type found.";

	ImGui::SetNextWindowPos(player_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::Begin("Select Players", w_open, window_flags);
	if(m_gamePlayers[m_selectedScenery].size() == 0)
		ImGui::TextColored(ImColor(210, 0, 0), no_files_found.c_str());
	else
		for(GameObjectSelection& gos : m_gamePlayers[m_selectedScenery])
			ImGui::Selectable(gos.name.c_str(), &gos.selected);
	ImGui::End();
}

void LocalMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;

	switch (m_controller.getActiveGameWindow()) {
	case ActiveGameWindow::MAPSELECTION:
		if (m_sceneriesFound) {
			// TODO Scenery Preview View
			sf::View view(
				sf::FloatRect(0, 0, m_sceneries[m_selectedScenery]->getMap()->getImageWidth(),
				m_sceneries[m_selectedScenery]->getMap()->getImageHeight())
			);
			view.setViewport(sf::FloatRect(0.3, 0.3, 0.4, 0.4));
			target.draw(m_ps, states);
			target.setView(view);
			// Scenery Preview Draw
			m_sceneries[m_selectedScenery]->drawPreview(target, states);
		}

		m_controller.setDefaultView();
		target.draw(m_play, states);
		target.draw(m_sceneryName, states);
		target.draw(m_switchLeft, states);
		target.draw(m_switchRight, states);
		break;
	case ActiveGameWindow::INGAME:
		target.draw(m_gameWindow, states);
		break;
	default:
		m_controller.setActiveGameWindow(ActiveGameWindow::MAPSELECTION);
		break;
	}
};

const GameController& LocalMenu::getGameController() const {
	return m_gameController;
}

GameObject* LocalMenu::getGameObjectById(const std::string& id) const
{
	return m_gameController.getGameObjectById(id);
}

void LocalMenu::updateLog() const
{
	m_gameController.updateLog();
}
