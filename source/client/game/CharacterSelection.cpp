#include <client/game/CharacterSelection.h>

CharacterSelection::CharacterSelection(Controller& c) :
	m_c(c),
	m_gc(c.getGameController()),
	m_nc(c.getNetController())
{
	// add one default Charactermenu
	m_characters.push_back(Character{});
}

CharacterSelection::~CharacterSelection()
{}

void CharacterSelection::drawImgui()
{
	for(size_t i = 0; i < m_characters.size(); i++)
		drawImguiCharacterMenu(i);

	// imgui.h:595 or imgui_demo.cpp:187
	int window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoNav;
	bool* w_open = NULL; // hides close option

	auto window_pos = ImVec2(m_c.getSettings().toW(0.4f), m_c.getSettings().toH(0.9f));
	auto window_size = ImVec2(m_c.getSettings().toW(0.2f), m_c.getSettings().toH(0.08f));

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::Begin("Character-Selection:Add-Menu", w_open, window_flags);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(7.0f, 0.8f, 0.8f));

		if(ImGui::Button("Add Player")) {
			if(m_characters.size() < MAX_LOCAL_PLAYERS)
				m_characters.push_back(Character{});
		}
		ImGui::PopStyleColor(3);

		ImGui::Text("Toggle Character-Menu with <F1>");
	ImGui::End();
}

void CharacterSelection::drawImguiCharacterMenu(size_t index)
{
	Character& c = m_characters[index];
	// imgui.h:595 or imgui_demo.cpp:187
	int window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNav;
	bool* w_open = NULL; // hides close option

	const float width = 0.22;
	const float posX = index * (width + 0.01) + 0.05;
	auto window_pos = ImVec2(m_c.getSettings().toW(posX), m_c.getSettings().toH(0.2f));
	auto window_size = ImVec2(m_c.getSettings().toW(width), m_c.getSettings().toH(0.6f));

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	std::string menuName = "Character Menu: ";
	menuName += (c.player) ? c.player->getName() : std::to_string(index + 1);
	ImGui::Begin(menuName.c_str(), w_open, window_flags);
		ImGui::BeginGroup();
		ImGui::Text("Select a Character");
		ImGui::Separator();

		ImGui::Columns(2, "characaterdetails");
		ImGui::BeginChild("CharacterSelection", ImVec2(0, -3.3 * ImGui::GetFrameHeightWithSpacing()));
		for(auto& p : m_gc.getScenery().getPlayerSetupMaps()) {
			ImGui::PushID(p.first.c_str());
			std::string pname = p.second[Reader::DEFAULT_PARAGRAPH][GameObject::S_NAME];
			if(ImGui::Selectable(pname.c_str(), c.selectedCharacter == p.first)) {
				if(!c.player) {
					c.selectedCharacter = p.first;
					sf::Color color = Helper::toColor(p.second[Reader::DEFAULT_PARAGRAPH][GameObject::S_COLOR]);
					c.playerColor = ImColor(color.r, color.g, color.b, color.a);
				}
			}
			ImGui::PopID();
		}
		ImGui::EndChild();
		ImGui::NextColumn();

		if(!c.selectedCharacter.empty())
			ImGui::Image(*GameReader::loadTexture(m_gc.getScenery().getPlayerSetupMaps()[c.selectedCharacter][Reader::DEFAULT_PARAGRAPH][GameObject::S_TEXTURE_PATH]));

		ImGui::Text("Extensions:");
		if(!c.selectedCharacter.empty())
			for(auto& p : m_gc.getScenery().getPlayerSetupMaps()[c.selectedCharacter][GameObject::S_EXTENSIONS_PARAGRAPH])
				if(Helper::toBool(p.second))
					ImGui::BulletText("%s", p.first.c_str());
		ImGui::NextColumn();
		ImGui::Columns();

		ImGui::Separator();
		// TODO show more character infos
		// TODO add multiple characters from one pc (ImGui Menu as class)
		ImGui::InputText("Name", c.playerName, PLAYER_NAME_LENGTH);
		ImVec2 size = ImGui::GetItemRectSize();
		ImGui::SameLine();
		ImGui::ColorEdit3("Character-Color", (float*)&c.playerColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		const char* keybindingProfileNames[m_c.getSettings().getKeybindings().size()];
		size_t keybindingProfileCount = 0;
		for(auto& k : m_c.getSettings().getKeybindings()){
			keybindingProfileNames[keybindingProfileCount] = k.first.c_str();
			keybindingProfileCount++;
		}
		ImGui::Combo("Controls", &c.selectedKeybindingProfile, keybindingProfileNames, keybindingProfileCount);

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(7.0f, 0.8f, 0.8f));

		if(ImGui::Button((c.player) ? "CHANGE" : "JOIN", size)) {
			if(verifyCharacter(c)) {
				std::string keybindingName = keybindingProfileNames[c.selectedKeybindingProfile];
				m_nc.sendPlayerConfigReq(Net::PlayerConfigReq{
					(c.player) ? c.player->getIdentifier() : "",
					c.selectedCharacter,
					c.playerName,
					sf::Color(c.playerColor.x*255, c.playerColor.y*255, c.playerColor.z*255)
				}, [&, keybindingName](GameObject* go) {
					c.player = go;
					m_gc.setLocalPlayerKeybinding(go, m_c.getSettings().getKeybinding(keybindingName));
				});
			}
		}
		ImGui::PopStyleColor(3);
		ImGui::EndGroup();
	ImGui::End();
}

bool CharacterSelection::verifyCharacter(const Character& c)
{
	if(Helper::trim(c.playerName).empty()) {
		m_c.pushMessage("You have to enter a name.");
		return false;
	}
	if(c.selectedCharacter.empty()) {
		m_c.pushMessage("You have to select a character.");
		return false;
	}
	return true;
}

