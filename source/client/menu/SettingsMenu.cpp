#include <client/menu/SettingsMenu.h>

SettingsMenu::SettingsMenu(Controller& c) :
	m_ps(100),
	m_controller(c)
{
	m_header.setFont(m_controller.getSettings().getFont());
	m_header.setString("Settings");
	m_header.setCharacterSize(m_controller.getSettings().toF(50));
	m_header.setStyle(sf::Text::Bold | sf::Text::Underlined);
	m_header.setFillColor(sf::Color::Red);
	m_header.setPosition(m_controller.getSettings().toW(0.35f), m_controller.getSettings().toH(40));

	auto gb = m_header.getGlobalBounds();
	m_ps.setColor(sf::Color::White);
	m_ps.setDrawingType(sf::Quads);
	m_ps.setLifetime(sf::seconds(3));
	m_ps.setOrigin(gb.left, gb.top, gb.width, gb.height, Origin::ON_BORDER);
}

SettingsMenu::~SettingsMenu() {}

void SettingsMenu::event(sf::Event& event) {
	sf::RenderWindow& w = m_controller.getWindow();
	sf::Vector2i pixelPos = sf::Mouse::getPosition(w);
	sf::Vector2f worldPos = w.mapPixelToCoords(pixelPos);

	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			m_controller.setActiveMenu(ActiveMenu::MAIN);
		}
	}
}

void SettingsMenu::update(sf::Time ellapsed) {
	m_ps.update(ellapsed);
}

void SettingsMenu::drawImgui()
{
	// imgui.h:595 or imgui_demo.cpp:187
	int window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize |
											ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar;
	bool* w_open = NULL; // hides close option

	auto window_pos = ImVec2(m_controller.getSettings().toW(0.1f), m_controller.getSettings().toH(0.3f));
	auto window_size = ImVec2(m_controller.getSettings().toW(0.8f), m_controller.getSettings().toH(0.6f));

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::Begin("Settings", w_open, window_flags);
		// TODO imgui settings menu
		ImGui::TextColored(ImColor(210, 0, 0), "Settings are not implemented yet, but you can access them in res/settings.ini");
	ImGui::End();
}

void SettingsMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;

	m_controller.setDefaultView();

	target.draw(m_ps, states);
	target.draw(m_header, states);
};
