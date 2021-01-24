#include <client/game/CharacterSelection.h>

CharacterSelection::CharacterSelection(Controller& c) :
	m_c(c),
	m_gc(c.getGameController())
{}

CharacterSelection::~CharacterSelection()
{}

void CharacterSelection::drawImgui()
{

	// imgui.h:595 or imgui_demo.cpp:187
	int window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNav;
	bool* w_open = NULL; // hides close option

	auto window_pos = ImVec2(m_c.getSettings().toW(0.1f), m_c.getSettings().toH(0.2f));
	auto window_size = ImVec2(m_c.getSettings().toW(0.4f), m_c.getSettings().toH(0.6f));

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::Begin("Character-Selection Menu", w_open, window_flags);
		ImGui::BeginGroup();
		ImGui::Text("Select a Character");
		ImGui::Separator();
		ImGui::BeginChild("CharacterSelection", ImVec2(0, -2 * ImGui::GetFrameHeightWithSpacing())); // 2 lines at the bottom
		for(auto& p : m_gc.getScenery().getPlayerSetupMaps()) {
			ImGui::PushID(p.first.c_str());
			std::string pname = p.second[Reader::DEFAULT_PARAGRAPH][GameObject::S_NAME];
			if(ImGui::Selectable(pname.c_str(), m_selectedPlayer == p.first)) {
				m_selectedPlayer = p.first;
				sf::Color c = Helper::toColor(p.second[Reader::DEFAULT_PARAGRAPH][GameObject::S_COLOR]);
				m_playerColor = ImColor(c.r, c.g, c.b, c.a);
			}
			ImGui::PopID();
		}
		ImGui::EndChild();
		ImGui::Separator();
		// TODO show more character infos
		// TODO add multiple characters from one pc (ImGui Menu as class)
		ImGui::InputText("Name", m_playerName, PLAYER_NAME_LENGTH);
		ImVec2 size = ImGui::GetItemRectSize();
		ImGui::SameLine();
		ImGui::ColorEdit4("Character-Color", (float*)&m_playerColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(7.0f, 0.8f, 0.8f));

		if(ImGui::Button("JOIN", size)) {
			// TODO send character join
		}
		ImGui::PopStyleColor(3);
		ImGui::EndGroup();
	ImGui::End();
}

