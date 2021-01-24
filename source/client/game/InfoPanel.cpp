#include <client/game/InfoPanel.h>

InfoPanel::InfoPanel(Controller& c) :
	m_controller(c),
	m_gc(c.getGameController())
{}

InfoPanel::~InfoPanel()
{}

void InfoPanel::drawImgui()
{
	// imgui.h:595 or imgui_demo.cpp:187
	int window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoTitleBar;
	bool* w_open = NULL; // hides close option

	auto window_pos = ImVec2(m_controller.getSettings().toW(0.2f), m_controller.getSettings().toH(0.2f));
	auto window_size = ImVec2(m_controller.getSettings().toW(0.6f), m_controller.getSettings().toH(0.6f));

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::Begin("InfoPanel", w_open, window_flags);
		// TODO imgui settings menu
		ImGui::TextColored(ImColor(210, 0, 0), "InfoPanel is not implemented yet.");
	ImGui::End();
}

