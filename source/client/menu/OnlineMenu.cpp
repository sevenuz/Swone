#include "client/menu/OnlineMenu.h"

OnlineMenu::OnlineMenu(Controller& c) :
	m_ps(100),
	m_c(c)
{
	m_header.setFont(m_c.getSettings().getFont());
	m_header.setString("Online");
	m_header.setCharacterSize(m_c.getSettings().toF(50));
	m_header.setStyle(sf::Text::Bold | sf::Text::Underlined);
	m_header.setFillColor(sf::Color::Red);
	m_header.setPosition(m_c.getSettings().toW(0.35f), m_c.getSettings().toH(40));

	auto gb = m_header.getGlobalBounds();
	m_ps.setColor(sf::Color::White);
	m_ps.setDrawingType(sf::Quads);
	m_ps.setLifetime(sf::seconds(3));
	m_ps.setOrigin(gb.left, gb.top, gb.width, gb.height, Origin::ON_BORDER);

	m_joinLobbyCode.reserve(LOBBY_CODE_LENGTH);
	m_joinLobbyPassword.reserve(LOBBY_PASSWORD_LENGTH);

	GameReader::readSceneryMaps(m_c.getSettings().getResourceDirectory());
	GameReader::hashResDir(m_c.getSettings().getResourceDirectory()); // TODO here?
}

OnlineMenu::~OnlineMenu() {}

void OnlineMenu::event(sf::Event& event) {
	if(!m_modalMessageStack.empty())
		return;
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			m_c.popState();
		}
	}
}

void OnlineMenu::update(sf::Time ellapsed) {
	m_ps.update(ellapsed);
}

void OnlineMenu::drawJoinWindow()
{
	// imgui.h:595 or imgui_demo.cpp:187
	int window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNav;
	bool* w_open = NULL; // hides close option

	auto window_pos = ImVec2(m_c.getSettings().toW(0.05f), m_c.getSettings().toH(0.3f));
	auto window_size = ImVec2(m_c.getSettings().toW(0.4f), m_c.getSettings().toH(0.6f));

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::Begin("Join a Game", w_open, window_flags);
		ImGui::BeginGroup();
		ImGui::Text("Select a Lobby");
		ImGui::Separator();
		ImGui::BeginChild("Lobby-Selection", ImVec2(0, -4.4 * ImGui::GetFrameHeightWithSpacing())); // 4.4 lines at the bottom
		for(auto& p : m_lobbies) {
			ImGui::PushID(p.code.c_str());
			if(ImGui::Selectable(p.name.c_str(), m_joinLobbyCode == p.code))
				m_joinLobbyCode = p.code;
			ImGui::PopID();
		}
		ImGui::EndChild();
		ImGui::Separator();

		ImGui::InputText("Code", m_joinLobbyCode.data(), LOBBY_CODE_LENGTH);
		ImGui::InputText("Password", m_joinLobbyPassword.data(), LOBBY_PASSWORD_LENGTH);
		ImVec2 size = ImGui::GetItemRectSize();

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(111.0f, 0.58f, 0.45f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(111.0f, 0.68f, 0.55f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(111.0f, 0.78f, 0.65f));
		if(ImGui::Button("REFRESH")) {
			sendLobbyRefresh();
		}
		ImGui::PopStyleColor(3);

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(7.0f, 0.8f, 0.8f));

		if(ImGui::Button("JOIN", size)) {
			joinLobby();
		}
		ImGui::PopStyleColor(3);
		ImGui::EndGroup();
	ImGui::End();
}

void OnlineMenu::sendLobbyRefresh()
{
	std::thread([&](){
		sf::TcpSocket socket;
		try {
			sf::Socket::Status status = socket.connect(m_c.getSettings().getServerIpAddress(), m_c.getSettings().getPort());
			if (status != sf::Socket::Done) {
				throw Net::Status{Net::C_CONNECTION, "sendLobbyRefresh: Error while Connecting to " + m_c.getSettings().getServerAndPort()};
			}
			Net::Packet reqPacket(Net::T_LOBBY_REFRESH);
			if (socket.send(reqPacket) != sf::Socket::Done) {
				throw Net::Status{Net::C_SEND, "sendLobbyRefresh: Error while sending T_LOBBY_REFRESH"};
			}
			Net::Packet resPacket;
			if(socket.receive(resPacket) != sf::Socket::Done) {
				throw Net::Status{Net::C_RECEIVE, "sendLobbyRefresh: Error while receiving"};
			}
			if(resPacket.getType() == Net::T_LOBBY_REFRESH) {
				Net::LobbyRefresh ls;
				resPacket >> ls;
				m_lobbies = ls.lobbies;
			}
			if(resPacket.getType() == Net::T_ERROR) {
				Net::Status s;
				resPacket >> s;
				throw s;
			}
		} catch(Net::Status s) {
			m_modalMessageStack.push(s.message);
			Log::ger().log(std::to_string(s.code) + ": " + s.message, Log::Label::Error);
		}
		socket.disconnect();
	}).detach();
}

void OnlineMenu::joinLobby()
{
	if(m_joinLobbyCode.empty()) {
		std::string err = "You have to enter a Lobby-Code to join a Lobby.";
		m_modalMessageStack.push(err);
		Log::ger().log(err, Log::Label::Error);
		return;
	}

	Net::JoinLobbyReq jlr = Net::JoinLobbyReq{
		m_joinLobbyCode,
		m_joinLobbyPassword
	};
	std::thread(&OnlineMenu::sendJoinLobbyReq, this, jlr).detach();
}

void OnlineMenu::sendJoinLobbyReq(Net::JoinLobbyReq jlr)
{
	sf::TcpSocket socket;
	try {
		sf::Socket::Status status = socket.connect(m_c.getSettings().getServerIpAddress(), m_c.getSettings().getPort());
		if (status != sf::Socket::Done) {
			throw Net::Status{Net::C_CONNECTION, "sendJoinLobby: Error while Connecting to " + m_c.getSettings().getServerAndPort()};
		}
		Net::Packet reqPacket(Net::T_JOIN_LOBBY_REQ);
		reqPacket << jlr;
		if (socket.send(reqPacket) != sf::Socket::Done) {
			throw Net::Status{Net::C_SEND, "sendJoinLobby: Error while sending T_JOIN_LOBBY_REQ"};
		}
		Net::Packet resPacket;
		if(socket.receive(resPacket) != sf::Socket::Done) {
			throw Net::Status{Net::C_RECEIVE, "sendJoinLobby: Error while receiving"};
		}
		if(resPacket.getType() == Net::T_JOIN_LOBBY_ACK) {
			Net::JoinLobbyAck jla;
			resPacket >> jla;
			handleJoinLobbyAck(socket, jla);
		}
		if(resPacket.getType() == Net::T_ERROR) {
			Net::Status s;
			resPacket >> s;
			throw s;
		}
	} catch(Net::Status s) {
		m_modalMessageStack.push(s.message);
		Log::ger().log(std::to_string(s.code) + ": " + s.message, Log::Label::Error);
	}
	socket.disconnect();
}

void OnlineMenu::drawCreateWindow()
{
	// imgui.h:595 or imgui_demo.cpp:187
	int window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNav;
	bool* w_open = NULL; // hides close option

	auto window_pos = ImVec2(m_c.getSettings().toW(0.5f), m_c.getSettings().toH(0.3f));
	auto window_size = ImVec2(m_c.getSettings().toW(0.4f), m_c.getSettings().toH(0.6f));

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

		ImGui::InputText("Lobbyname", m_lobbyName, LOBBY_NAME_LENGTH);
		ImGui::InputText("Password", m_lobbyPassword, LOBBY_PASSWORD_LENGTH);
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
	std::string lobbyName = Helper::trim(std::string(m_lobbyName));
	if(lobbyName.empty()) {
		std::string err = "You have to choose a Lobby-Name to create a Lobby.";
		m_modalMessageStack.push(err);
		Log::ger().log(err, Log::Label::Error);
		return;
	}

	auto& p = GameReader::getSceneryMaps()[m_selectedScenery];
	Scenery scenery(m_c.getSettings().getResourceDirectory(), Helper::parseFileName(m_selectedScenery), p);

	Net::CreateLobbyReq clr = Net::CreateLobbyReq{
		lobbyName,
		std::string(m_lobbyPassword),
		scenery.getFileCheck()
	};
	std::thread(&OnlineMenu::sendCreateLobbyReq, this, clr).detach();
}

void OnlineMenu::sendCreateLobbyReq(Net::CreateLobbyReq clr)
{
	sf::TcpSocket socket;
	try {
		sf::Socket::Status status = socket.connect(m_c.getSettings().getServerIpAddress(), m_c.getSettings().getPort());
		if (status != sf::Socket::Done) {
			throw Net::Status{Net::C_CONNECTION, "sendLobbyRequest: Error while Connecting to " + m_c.getSettings().getServerAndPort()};
		}
		Net::Packet reqPacket(Net::T_CREATE_LOBBY);
		reqPacket << clr;
		if (socket.send(reqPacket) != sf::Socket::Done) {
			throw Net::Status{Net::C_SEND, "sendLobbyRequest: Error while sending T_CREATE_LOBBY"};
		}

		Net::Packet resPacket;
		if(socket.receive(resPacket) != sf::Socket::Done) {
			throw Net::Status{Net::C_RECEIVE, "sendLobbyRequest: Error while receiving"};
		}

		if(resPacket.getType() == Net::T_FILE_REQUEST) {
			Net::GameFileCheckAnswer res;
			resPacket >> res;
			m_modalMessageStack.push("Server needs files...");
			Net::sendMissingFiles(socket, clr.fileCheck, res);
			resPacket = Net::Packet();
			if(socket.receive(resPacket) != sf::Socket::Done) {
				throw Net::Status{Net::C_RECEIVE, "sendLobbyRequest: Error while receiving T_JOIN_LOBBY_ACK"};
			}
		}

		if(resPacket.getType() == Net::T_JOIN_LOBBY_ACK) {
			Net::JoinLobbyAck jla;
			resPacket >> jla;
			handleJoinLobbyAck(socket, jla);
		}

		if(resPacket.getType() == Net::T_ERROR) {
			Net::Status s;
			resPacket >> s;
			throw s;
		}
	} catch(Net::Status s) {
		m_modalMessageStack.push(s.message);
		Log::ger().log(std::to_string(s.code) + ": " + s.message, Log::Label::Error);
	}
	socket.disconnect();
}

void OnlineMenu::handleJoinLobbyAck(sf::TcpSocket& socket, Net::JoinLobbyAck jla)
{
	Net::handleGameFileCheck(socket, jla.fileCheck, m_c.getSettings().getDownloadDirectory());

	m_c.loadGame(jla);
	m_c.pushState(Controller::State::Game);

	m_modalMessageStack.push("Join Lobby");
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

	m_c.setDefaultView();
	target.draw(m_ps, states);
	target.draw(m_header, states);
};
