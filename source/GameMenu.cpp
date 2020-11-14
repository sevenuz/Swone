#include <GameMenu.h>

GameMenu::GameMenu(Controller& c) :m_ps(100), m_controller(c), m_gameController(c), m_gameWindow(m_gameController) {

	Log::ger().log("GamePanel created");

	m_ps.setColor(sf::Color::White);
	m_ps.setDrawingType(sf::Quads);
	m_ps.setLifetime(sf::seconds(3));
	m_ps.setOrigin(m_controller.getSettings().toW(0.4f), m_controller.getSettings().toH(61), 150, 3, Origin::ON_BORDER);

	m_play.setFont(m_controller.getSettings().getFont());
	m_play.setString("Play");
	m_play.setPosition(m_controller.getSettings().toW(0.415f), m_controller.getSettings().toH(0.8f));
	m_play.setFillColor(sf::Color::Yellow);
	m_play.setCharacterSize(m_controller.getSettings().toF(50));

	m_mapName.setFont(m_controller.getSettings().getFont());
	m_mapName.setPosition(m_controller.getSettings().toW(0.4f), m_controller.getSettings().toH(31));
	m_mapName.setFillColor(sf::Color::White);
	m_mapName.setCharacterSize(m_controller.getSettings().toF(30));

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

	readMapsFromDir();
}

GameMenu::~GameMenu() {
	// TODO delete Map vector
	Log::ger().log("GamePanel destroyed");
}

void GameMenu::readMapsFromDir() {
	tinydir_dir dir;
	tinydir_open(&dir, m_controller.getSettings().getMapDirectory().c_str());

	if (!dir.has_next) {
		Log::ger().log("no files or no dir available.", Log::Label::Error);
		throw std::invalid_argument("no files or no dir available");
	}
	else {
		m_mapsFound = true;
	}

	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);

		if (!file.is_dir)
		{
			Log::ger().log(file.name);
			Map* map = new Map(m_controller);

			std::stringstream ss;
			ss << m_controller.getSettings().getMapDirectory() << file.name;
			m_mapReader.setPath(ss.str());

			m_mapReader.setMap(map);
			m_mapReader.read();

			m_maps.push_back(map);
		}

		tinydir_next(&dir);
	}

	setMapSelection(0);
}

void GameMenu::setMapSelection(int i) {
	if (!m_mapsFound) {
		Log::ger().log("no maps found.", Log::Label::Error);
		return;
	}

	if (i >= (int)m_maps.size()) {
		i = 0;
	}
	else if (i < 0) {
		i = m_maps.size() - 1;
	}

	m_selectedMap = i;

	m_mapName.setString(m_maps[m_selectedMap]->getName());
	Log::ger().log(m_maps[m_selectedMap]->getName());

	m_gameController.setMap(m_maps[m_selectedMap]);
}

void GameMenu::setActionSelection(char i) {
	if (i > GamePanelMenuPoint::LAST) {
		i = GamePanelMenuPoint::FIRST;
	}
	else if (i < GamePanelMenuPoint::FIRST) {
		i = GamePanelMenuPoint::LAST;
	}
	m_selectedAction = i;
}

void GameMenu::startGame() {
	if (m_mapsFound) {
		m_controller.setActiveGameWindow(ActiveGameWindow::INGAME);
	}
	else {
		Log::ger().log("No Map. Can't start the game.", Log::Label::Error);
	}
}

void GameMenu::event(sf::Event& event) {
	sf::RenderWindow& w = m_controller.getWindow();
	sf::Vector2i pixelPos = sf::Mouse::getPosition(w);
	sf::Vector2f worldPos = w.mapPixelToCoords(pixelPos);

	switch (m_controller.getActiveGameWindow()) {
	case ActiveGameWindow::MAPSELECTION:
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) {
				m_controller.setActiveWindow(ActiveWindow::MAINMENU);
			}
			if (event.key.code == sf::Keyboard::Left) {
				setMapSelection(m_selectedMap - 1);
			}
			if (event.key.code == sf::Keyboard::Right) {
				setMapSelection(m_selectedMap + 1);
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
				setMapSelection(m_selectedMap - 1);
			}
			else if (m_switchRight.getGlobalBounds().contains(worldPos.x, worldPos.y)) {
				setMapSelection(m_selectedMap + 1);
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

void GameMenu::update(sf::Time ellapsed) {
	m_ps.update(ellapsed);
	switch (m_controller.getActiveGameWindow()) {
	case ActiveGameWindow::MAPSELECTION:
		//m_maps[m_selectedMap]->getSprite().setScale(sf::Vector2f(0.25, 0.25));
		break;
	case ActiveGameWindow::INGAME:
		m_gameWindow.update(ellapsed);
		break;
	default:
		break;
	}
}

void GameMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;
	sf::View view(sf::FloatRect(0, 0, m_maps[m_selectedMap]->getImageWidth(), m_maps[m_selectedMap]->getImageHeight()));
	view.setViewport(sf::FloatRect(0.3, 0.3, 0.4, 0.4));

	switch (m_controller.getActiveGameWindow()) {
	case ActiveGameWindow::MAPSELECTION:
		target.draw(m_ps, states);
		m_controller.setView(view);

		if (m_mapsFound) {
			target.draw(m_maps[m_selectedMap]->getSprite(), states);
		}

		m_controller.setDefaultView();
		target.draw(m_play, states);
		target.draw(m_mapName, states);
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
}

