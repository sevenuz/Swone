#include <client/menu/OnlineMenu.h>

OnlineMenu::OnlineMenu(Controller& c) :
	m_ps(100),
	m_controller(c),
	m_gameWindow(c, m_gameController),
	m_gameReader(c.getSettings().getResourceDirectory())
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

	m_sceneriesFound = m_gameReader.getSceneries().size() > 0;
	if(m_sceneriesFound)
		setScenerySelection(0);

	for(GameObject* o : m_gameReader.getPlayers()) {
		const std::string type = o->getType();
		if(type == GameObject::S_PLAYER_TYPE) {
			m_gamePlayers.push_back({o, true});
		}
	}
}

OnlineMenu::~OnlineMenu() {}

void OnlineMenu::setScenerySelection(int i) {
	if (!m_sceneriesFound) {
		Log::ger().log("no sceneries found.", Log::Label::Error);
		return;
	}

	if (i >= (int)m_gameReader.getSceneries().size()) {
		i = 0;
	}
	else if (i < 0) {
		i = m_gameReader.getSceneries().size() - 1;
	}

	m_selectedScenery = i;

	m_sceneryName.setString(m_gameReader.getSceneries()[m_selectedScenery]->getName());
}

void OnlineMenu::setActionSelection(char i) {
	if (i > GamePanelMenuPoint::LAST) {
		i = GamePanelMenuPoint::FIRST;
	}
	else if (i < GamePanelMenuPoint::FIRST) {
		i = GamePanelMenuPoint::LAST;
	}
	m_selectedAction = i;
}

void OnlineMenu::startGame() {
	if (m_sceneriesFound) {
		m_gameController.clearPlayers();
		m_gameController.setScenery(m_gameReader.getSceneries()[m_selectedScenery]);
		m_gameWindow.setViewZoom();
		for(GameObjectSelection& gos : m_gamePlayers)
			if(gos.selected)
				m_gameController.pushPlayer(gos.obj);
		m_gameController.startGame();
		m_controller.setActiveGameWindow(ActiveGameWindow::INGAME);
	}
	else {
		Log::ger().log("No Scenery. Can't start the game.", Log::Label::Error);
	}
}

void OnlineMenu::event(sf::Event& event) {
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

void OnlineMenu::update(sf::Time ellapsed) {
	m_ps.update(ellapsed);
	switch (m_controller.getActiveGameWindow()) {
	case ActiveGameWindow::MAPSELECTION:
		//m_gameReader.getSceneries()[m_selectedScenery]->getSprite().setScale(sf::Vector2f(0.25, 0.25));
		break;
	case ActiveGameWindow::INGAME:
		m_gameWindow.update(ellapsed);
		break;
	default:
		break;
	}
}

void OnlineMenu::drawImgui()
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
	if(m_gamePlayers.size() == 0)
		ImGui::TextColored(ImColor(210, 0, 0), no_files_found.c_str());
	else
		for(GameObjectSelection& gos : m_gamePlayers)
			ImGui::Selectable(gos.obj->getIdentifier().c_str(), &gos.selected);
	ImGui::End();
}

void OnlineMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;

	switch (m_controller.getActiveGameWindow()) {
	case ActiveGameWindow::MAPSELECTION:
		if (m_sceneriesFound) {
			sf::View view(
				sf::FloatRect(0, 0, m_gameReader.getSceneries()[m_selectedScenery]->getMap()->getImageWidth(),
				m_gameReader.getSceneries()[m_selectedScenery]->getMap()->getImageHeight())
			);
			view.setViewport(sf::FloatRect(0.3, 0.3, 0.4, 0.4));
			target.draw(m_ps, states);
			target.setView(view);
			target.draw(*m_gameReader.getSceneries()[m_selectedScenery], states);
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

const GameController& OnlineMenu::getGameController() const {
	return m_gameController;
}

GameObject* OnlineMenu::getGameObjectById(const std::string& id) const
{
	return m_gameController.getGameObjectById(id);
}

void OnlineMenu::updateLog() const
{
	m_gameController.updateLog();
}
