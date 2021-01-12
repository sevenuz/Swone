#include <client/menu/OnlineMenu.h>

OnlineMenu::OnlineMenu(Controller& c) :
	m_ps(100),
	m_controller(c)
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

	GameReader::readSceneryMaps(m_controller.getSettings().getResourceDirectory());
	GameReader::hashResDir(m_controller.getSettings().getResourceDirectory()); // TODO here?
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
		ImGui::BeginChild("Scenery-Selection", ImVec2(0, -4.4 * ImGui::GetFrameHeightWithSpacing())); // 4.4 lines at the bottom
		for(auto& p : GameReader::getSceneryMaps()) {
			std::string name = p.second[Reader::DEFAULT_PARAGRAPH][Scenery::S_NAME];
			if (ImGui::Selectable(name.c_str(), m_selectedScenery == p.first))
				m_selectedScenery = p.first;
		}
		ImGui::EndChild();
		ImGui::Separator();

		ImGui::InputText("Lobbyname", m_lobbyName.data(), 32);
		ImGui::InputText("Password", m_lobbyPassword.data(), 32);
		ImVec2 size = ImGui::GetItemRectSize();

		ImGui::Checkbox("Private", &m_lobbyPrivate); // TODO add to CreateLobbyRequest

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
	if(m_selectedScenery.empty()) {
		std::string err = "You have to select a Scenery to create a Lobby.";
		m_modalMessageStack.push(err);
		Log::ger().log(err, Log::Label::Error);
		return;
	}
	if(m_lobbyName.empty()) {
		std::string err = "You have to choose a Lobby-Name to create a Lobby.";
		m_modalMessageStack.push(err);
		Log::ger().log(err, Log::Label::Error);
		return;
	}

	auto& p = GameReader::getSceneryMaps()[m_selectedScenery];
	Scenery scenery(m_controller.getSettings().getResourceDirectory(), Helper::parseFileName(m_selectedScenery), p);

	Net::CreateLobbyReq clr = Net::CreateLobbyReq{
		m_lobbyName,
		m_lobbyPassword,
		scenery.getFileCheck()
	};
	std::thread(&OnlineMenu::sendLobbyRequest, this, clr).detach();
}

void OnlineMenu::sendLobbyRequest(Net::CreateLobbyReq clr)
{
	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect(m_controller.getSettings().getServerIpAddress(), m_controller.getSettings().getPort());
	if (status != sf::Socket::Done) {
		std::string err = "sendLobbyRequest: Error while Connecting to " + m_controller.getSettings().getServerAndPort();
		m_modalMessageStack.push(err);
		Log::ger().log(err, Log::Label::Error);
		return;
	}
	Net::Packet reqPacket(Net::T_CREATE_LOBBY);
	reqPacket << clr;
	if (socket.send(reqPacket) != sf::Socket::Done) {
		std::string err = "sendLobbyRequest: Error while Sending to " + m_controller.getSettings().getServerAndPort();
		m_modalMessageStack.push(err);
		Log::ger().log(err, Log::Label::Error);
		return;
	}

	Net::Packet resPacket;
	if(socket.receive(resPacket) != sf::Socket::Done) {
		std::string err = "sendLobbyRequest: Error while Receiving to " + m_controller.getSettings().getServerAndPort();
		m_modalMessageStack.push(err);
		Log::ger().log(err, Log::Label::Error);
		return;
	}

	Net::CreateLobbyRes res;
	switch((int)resPacket.getType()) {
		case Net::T_CREATE_LOBBY:
			resPacket >> res;
			m_modalMessageStack.push("Server needs files...");
			Net::sendMissingFiles(socket, clr.fileCheck, res);
			break;
		case Net::T_JOIN_LOBBY_ACK:
			m_modalMessageStack.push("Join Lobby");
			break;
	}
	socket.disconnect();
}

void OnlineMenu::drawImgui()
{
	drawJoinWindow();
	drawCreateWindow();

	if (!m_modalMessageStack.empty())
			ImGui::OpenPopup("Message:");
	if (ImGui::BeginPopupModal("Message:", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
			ImGui::Text(m_modalMessageStack.top().c_str());
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120,0))) {
				m_modalMessageStack.pop();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::EndPopup();
	}
}

void OnlineMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;

	m_controller.setDefaultView();

	target.draw(m_ps, states);
	target.draw(m_header, states);
};
