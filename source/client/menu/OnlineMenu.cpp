#include <client/menu/OnlineMenu.h>

OnlineMenu::OnlineMenu(Controller& c, GameReader& gr) :
	m_ps(100),
	m_controller(c),
	m_gameReader(gr)
{
	m_header.setFont(m_controller.getSettings().getFont());
	m_header.setString("Online");
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

OnlineMenu::~OnlineMenu() {}

void OnlineMenu::event(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			m_controller.setActiveMenu(ActiveMenu::MAIN);
		}
	}
}

void OnlineMenu::update(sf::Time ellapsed) {
	m_ps.update(ellapsed);
}

void OnlineMenu::drawJoinWindow()
{
	// imgui.h:595 or imgui_demo.cpp:187
	int window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize |
											ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
	bool* w_open = NULL; // hides close option

	auto window_pos = ImVec2(m_controller.getSettings().toW(0.05f), m_controller.getSettings().toH(0.3f));
	auto window_size = ImVec2(m_controller.getSettings().toW(0.4f), m_controller.getSettings().toH(0.6f));

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::Begin("Join a Game", w_open, window_flags);
		// TODO imgui online joining menu
		ImGui::TextColored(ImColor(210, 0, 0), "Joining is not possible yet.");
	ImGui::End();
}

void OnlineMenu::drawCreateWindow()
{
	// imgui.h:595 or imgui_demo.cpp:187
	int window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize |
											ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
	bool* w_open = NULL; // hides close option

	auto window_pos = ImVec2(m_controller.getSettings().toW(0.5f), m_controller.getSettings().toH(0.3f));
	auto window_size = ImVec2(m_controller.getSettings().toW(0.4f), m_controller.getSettings().toH(0.6f));

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::Begin("Create a Lobby", w_open, window_flags);
		ImGui::BeginGroup();
		ImGui::Text("Select a Scenery");
		ImGui::Separator();
		ImGui::BeginChild("Scenery-Selection", ImVec2(0, -3 * ImGui::GetFrameHeightWithSpacing())); // 3 lines at the bottom
		for(int i = 0; i < (int)m_gameReader.getSceneries().size(); i++)
			if (ImGui::Selectable(m_gameReader.getSceneries()[i]->getName().c_str(), m_selectedScenery == i))
				m_selectedScenery = i;
		ImGui::EndChild();

		ImGui::InputText("Lobbyname", m_lobbyName.data(), 32);
		ImGui::InputText("Password", m_lobbyPassword.data(), 32);
		ImVec2 size = ImGui::GetItemRectSize();

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(7.0f, 0.8f, 0.8f));
		if(ImGui::Button("CREATE", size)) {
			createLobby();
		}
		ImGui::PopStyleColor(3);
		ImGui::EndGroup();
	ImGui::End();
}

void OnlineMenu::createLobby()
{
	if(m_selectedScenery < 0) {
		Log::ger().log("You need a Scenerey", Log::Label::Error);
		return;
	}
	if(m_lobbyName.empty()) {
		Log::ger().log("You need a Lobbyname", Log::Label::Error);
		return;
	}
	for(auto& s : m_gameReader.getSceneries()[m_selectedScenery]->getFileList()) {
		Log::ger().log(s.first + " : " + s.second);
	}
	//std::thread serverThread(&Server::start, &server);
}

void OnlineMenu::drawImgui()
{
	drawJoinWindow();
	drawCreateWindow();
}

void OnlineMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;

	m_controller.setDefaultView();

	target.draw(m_ps, states);
	target.draw(m_header, states);
};
