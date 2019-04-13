#include <MainMenu.h>

MainMenu::MainMenu(Controller& c):m_ps(100), m_controller(c) {

    m_controller.pushLogMsg("MainMenu created");

    m_ps.setColor(sf::Color::White);
    m_ps.setDrawingType(sf::Quads);
    m_ps.setLifetime(sf::seconds(3));

    m_selectedMenupoint = Menupoint::LOKAL;

    m_header.setString("Swone");
    m_header.setCharacterSize(Settings::toF(TEXT_HEADER_SIZE));
    m_header.setStyle(sf::Text::Bold | sf::Text::Underlined);
    m_header.setColor(sf::Color::Red);
    m_header.setPosition(Settings::toW(TEXT_MARGIN_LEFT),Settings::toH(TEXT_MARGIN_TOP));

    m_lokal.setString("Lokal");
    m_lokal.setPosition(Settings::toW(TEXT_MARGIN_LEFT),Settings::toH(TEXT_MARGIN_TOP * 2));

    m_online.setString("Online");
    m_online.setPosition(Settings::toW(TEXT_MARGIN_LEFT),Settings::toH(TEXT_MARGIN_TOP * 3));

    m_settings.setString("Settings");
    m_settings.setPosition(Settings::toW(TEXT_MARGIN_LEFT),Settings::toH(TEXT_MARGIN_TOP * 4));

    m_exit.setString("Exit");
    m_exit.setPosition(Settings::toW(TEXT_MARGIN_LEFT),Settings::toH(TEXT_MARGIN_TOP * 5));

    changeSelectedMenupoint();
}

MainMenu::~MainMenu() {
    m_controller.pushLogMsg("MainMenu destroyed");
}

void MainMenu::changeFont(sf::Font & font) {
    m_header.setFont(font);
    m_lokal.setFont(font);
    m_online.setFont(font);
    m_settings.setFont(font);
    m_exit.setFont(font);
    changeSelectedMenupoint();
}

void MainMenu::selectedStyle(sf::Text& t) {
    t.setCharacterSize(Settings::toF(TEXT_SELECTED_SIZE));
    t.setColor(sf::Color::Yellow);
    m_ps.setOrigin(t.getGlobalBounds().left,t.getGlobalBounds().top,t.getGlobalBounds().width,t.getGlobalBounds().height,Origin::ON_BORDER);
}

void MainMenu::deselectedStyle(sf::Text& t) {
    t.setCharacterSize(Settings::toF(TEXT_MENUPOINT_SIZE));
    t.setColor(sf::Color::White);
}

void MainMenu::changeSelectedMenupoint() {
    deselectedStyle(m_lokal);
    deselectedStyle(m_online);
    deselectedStyle(m_settings);
    deselectedStyle(m_exit);
    switch(m_selectedMenupoint) {
    case Menupoint::LOKAL:
        selectedStyle(m_lokal);
        break;
    case Menupoint::ONLINE:
        selectedStyle(m_online);
        break;
    case Menupoint::SETTINGS:
        selectedStyle(m_settings);
        break;
    case Menupoint::EXIT:
        selectedStyle(m_exit);
        break;
    default:
        m_selectedMenupoint = Menupoint::LOKAL;
        changeSelectedMenupoint();
    }
}

void MainMenu::setSelectedMenupoint(unsigned int smp) {
    m_selectedMenupoint = smp;
    changeSelectedMenupoint();
}

void MainMenu::setFont(sf::Font & font) {
    changeFont(font);
}

void MainMenu::m_lokalPressed() {
    m_controller.setActiveWindow(ActiveWindow::GAME);
}
void MainMenu::m_onlinePressed() {}
void MainMenu::m_settingsPressed() {}
void MainMenu::m_exitPressed() {
    m_controller.getWindow().close();
}

void MainMenu::event(sf::Event& event) {
    sf::RenderWindow & w = m_controller.getWindow();
	sf::Vector2i pixelPos = sf::Mouse::getPosition(w);
	sf::Vector2f worldPos = w.mapPixelToCoords(pixelPos);

    if(event.type == sf::Event::KeyPressed) {
        if(event.key.code == sf::Keyboard::Down) {
            if(m_selectedMenupoint == Menupoint::LAST)
                m_selectedMenupoint = Menupoint::FIRST;
            else
                m_selectedMenupoint += 1;
            changeSelectedMenupoint();
        }
        if(event.key.code == sf::Keyboard::Up) {
            if(m_selectedMenupoint == Menupoint::FIRST)
                m_selectedMenupoint = Menupoint::LAST;
            else
                m_selectedMenupoint -= 1;
            changeSelectedMenupoint();
        }
        if(event.key.code == sf::Keyboard::Return) {//Enter
            if(m_selectedMenupoint == Menupoint::LOKAL) {
                m_lokalPressed();
            }
            if(m_selectedMenupoint == Menupoint::ONLINE) {
                m_onlinePressed();
            }
            if(m_selectedMenupoint == Menupoint::SETTINGS) {
                m_settingsPressed();
            }
            if(m_selectedMenupoint == Menupoint::EXIT) {
                m_exitPressed();
            }
        }
    } else if(event.type == sf::Event::MouseMoved) {
        if(m_lokal.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
            setSelectedMenupoint(Menupoint::LOKAL);
        } else if(m_online.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
            setSelectedMenupoint(Menupoint::ONLINE);
        } else if(m_settings.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
            setSelectedMenupoint(Menupoint::SETTINGS);
        } else if(m_exit.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
            setSelectedMenupoint(Menupoint::EXIT);
        }
    } else if(event.type == sf::Event::MouseButtonReleased) {
    	//sf::Mouse::getPosition(w).x,sf::Mouse::getPosition(w).y)

        if(m_lokal.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
            m_lokalPressed();
        } else if(m_online.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
            m_onlinePressed();
        } else if(m_settings.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
            m_settingsPressed();
        } else if(m_exit.getGlobalBounds().contains(worldPos.x,worldPos.y)) {
            m_exitPressed();
        }
    }
}

void MainMenu::update(sf::Time ellapsed) {
    m_ps.update(ellapsed);
}

void MainMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;

	target.draw(m_ps, states);
	target.draw(m_header, states);
	target.draw(m_lokal, states);
	target.draw(m_online, states);
	target.draw(m_settings, states);
	target.draw(m_exit, states);
};
