#include <GamePanel.h>

GamePanel::GamePanel(Controller& c) : m_controller(c), m_gameController(c), m_gameWindow(m_gameController), m_mapReader(c) {
//m_ps(100)
	m_controller.pushLogMsg("GamePanel created");

/*
	m_ps.setColor(sf::Color::White);
	m_ps.setDrawingType(sf::Quads);
	m_ps.setLifetime(sf::seconds(3));
	//m_ps.setOrigin(Settings::toW(0.1f), Settings::toH(0.1f), 580, 300, Origin::ON_BORDER);
*/

	m_play.setFont(m_controller.settings.font); // @suppress("Invalid arguments")
	m_play.setString("Play");
	m_play.setPosition(Settings::toW(0.1f), Settings::toH(0.5f));
	m_play.setColor(sf::Color::Yellow);
	m_play.setCharacterSize(Settings::toF(50));

	m_mapName.setFont(m_controller.settings.font); // @suppress("Invalid arguments")
	m_mapName.setPosition(Settings::toW(360), Settings::toH(31));
	m_mapName.setColor(sf::Color::White);
	m_mapName.setCharacterSize(Settings::toF(30));

	m_switchLeft.setFont(m_controller.settings.font); // @suppress("Invalid arguments")
	m_switchLeft.setString("<");
	m_switchLeft.setPosition(Settings::toW(30), Settings::toH(150));
	m_switchLeft.setColor(sf::Color::Yellow);
	m_switchLeft.setCharacterSize(Settings::toF(50));

	m_switchRight.setFont(m_controller.settings.font); // @suppress("Invalid arguments")
	m_switchRight.setString(">");
	m_switchRight.setPosition(Settings::toW(590), Settings::toH(150));
	m_switchRight.setColor(sf::Color::Yellow);
	m_switchRight.setCharacterSize(Settings::toF(50));

	readMapsFromDir();
}

GamePanel::~GamePanel() {
	m_controller.pushLogMsg("GamePanel destroyed");
}

void GamePanel::readMapsFromDir() {
	tinydir_dir dir;
	tinydir_open(& dir, m_controller.settings.mapDir);

	if(!dir.has_next) {
		m_controller.pushLogMsg("no files or no dir available.", "error");
		throw std::invalid_argument("no files or no dir available");
	} else {
		m_mapsFound = true;
	}

	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);

		if (! file.is_dir)
		{
			m_controller.pushLogMsg(file.name);
			Map * map = new Map(m_controller);

			std::stringstream ss;
			ss << file.name;
			m_mapReader.setPath(m_controller.settings.mapDir + ss.str());

			m_mapReader.setMap(map);
			m_mapReader.readMap();

			m_maps.push_back(map);
		}

		tinydir_next(&dir);
	}

	setMapSelection(0);
}

void GamePanel::setMapSelection(int i){
	if(! m_mapsFound){
		m_controller.pushLogMsg("no maps found.", "error");
		return;
	}

	if(i >= (int) m_maps.size()) {
		i = 0;
	}
	else if(i < 0) {
		i = m_maps.size() - 1;
	}

	m_selectedMap = i;

	//m_ps.setOrigin(Settings::toW(0.1f), Settings::toH(0.1f), m_maps[m_selectedMap]->getScale() * 0.4 * m_maps[m_selectedMap]->getImageWidth(), m_maps[m_selectedMap]->getScale() * 0.4 * m_maps[m_selectedMap]->getImageHeight(), Origin::ON_BORDER);

	m_mapName.setString(m_maps[m_selectedMap]->getName());
	m_controller.pushLogMsg(m_maps[m_selectedMap]->getName());

	m_gameController.setMap(m_maps[m_selectedMap]);
}

void GamePanel::setActionSelection(int i){
	if(i > GamePanelMenuPoint::LAST) {
		i = GamePanelMenuPoint::FIRST;
	} else if(i < GamePanelMenuPoint::FIRST) {
		i = GamePanelMenuPoint::LAST;
	}
	m_selectedAction = i;
}

void GamePanel::startGame(){
	if(m_mapsFound){
		m_controller.pushLogMsg("start game");
		m_controller.setActiveGameWindow(ActiveGameWindow::GAME);	
	} else {
		m_controller.pushLogMsg("No Map. Can't start the game.", "error");
	}
}

void GamePanel::event(sf::Event& event) {
	sf::RenderWindow & w = m_controller.getWindow();
	sf::Vector2i pixelPos = sf::Mouse::getPosition(w);
	sf::Vector2f worldPos = w.mapPixelToCoords(pixelPos);

	switch(m_controller.getActiveGameWindow()) {
		case ActiveGameWindow::MAPSELECTION:
			if(event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
						m_controller.setActiveWindow(ActiveWindow::MAINMENU);
				}
				if (event.key.code == sf::Keyboard::Left) {
					setMapSelection(m_selectedMap - 1);
				}
				if (event.key.code == sf::Keyboard::Right) {
					setMapSelection(m_selectedMap + 1);
				}
				if(event.key.code == sf::Keyboard::Down) {
					setActionSelection(m_selectedMap + 1);
				}
				if(event.key.code == sf::Keyboard::Up) {
					setActionSelection(m_selectedMap - 1);
				}
				if(event.key.code == sf::Keyboard::P) {
					//Spawn Player
				}
				if(event.key.code == sf::Keyboard::Return) {//Enter
					startGame();
				}
			} else if(event.type == sf::Event::MouseButtonReleased) {
				if(m_play.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
					startGame();
				} else if(m_switchLeft.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
					setMapSelection(m_selectedMap - 1);
				} else if(m_switchRight.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
					setMapSelection(m_selectedMap + 1);
				}
			}
			break;
		case ActiveGameWindow::GAME:
			m_gameWindow.event(event);
			break;
		default:
			break;
		}
}

void GamePanel::update(sf::Time ellapsed) {
	//m_ps.update(ellapsed);
	switch(m_controller.getActiveGameWindow()) {
	case ActiveGameWindow::MAPSELECTION:
		//m_maps[m_selectedMap]->getSprite().setScale(sf::Vector2f(0.25, 0.25));
		break;
	case ActiveGameWindow::GAME:
		m_gameWindow.update(ellapsed);
		break;
	default:
		break;
	}
}

void GamePanel::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	states.transform *= getTransform();
	states.texture = NULL;
	sf::View view(sf::FloatRect(0,0,m_controller.settings.WIDTH,m_controller.settings.HEIGHT));
	view.setViewport(sf::FloatRect(0.1, 0.1, 0.4, 0.4));

	switch(m_controller.getActiveGameWindow()) {
	case ActiveGameWindow::MAPSELECTION:
		//target.draw(m_ps, states);
		m_controller.getWindow().setView(view);

		if(m_mapsFound){
			target.draw(m_maps[m_selectedMap]->getSprite(), states);
		}
		
		m_controller.getWindow().setView(m_controller.getWindow().getDefaultView());
		target.draw(m_play, states);
		target.draw(m_mapName, states);
		target.draw(m_switchLeft, states);
		target.draw(m_switchRight, states);
		break;
	case ActiveGameWindow::GAME:
		target.draw(m_gameWindow, states);
		break;
	default:
		m_controller.setActiveGameWindow(ActiveGameWindow::MAPSELECTION);
		break;
	}
};
