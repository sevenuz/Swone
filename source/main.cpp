#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <string>

#include "MainMenu.h"
#include "GamePanel.h"
#include "graphics/ParticleSystem.h"
#include "Controller.h"
#include "Settings.h"
#include "util/reader/Reader.h"
#include "util/Log.h"

/*
//Defines damit ASIO_STANDALONE und damit socketio++ gehen
//Sie sind im socketio_impl header eingebunden

#define WIN32_WINNT 0x0501
#define WINVER 0x0501 //Windows XP Kompatibel
#define ASIO_STANDALONE
#define _WEBSOCKETPP_MINGW_THREAD_
#define _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_CPP11_FUNCTIONAL_
*/


sf::RenderWindow m_window(sf::VideoMode(Settings::WIDTH, Settings::HEIGHT, Settings::STANDART_BITS_PER_PIXEL), "SFML Test");
Controller m_controller(m_window);
MainMenu m_menu(m_controller);
GamePanel m_gamePanel(m_controller);
sf::Clock m_clock;

bool KEY_STRG_PRESSED = false;
bool KEY_L_PRESSED = false;

void iniSettings() {
	m_menu.setFont(m_controller.settings.font); // @suppress("Invalid arguments")
	m_window.setVerticalSyncEnabled(Settings::STANDART_VERTICAL_SYNC_ENABLED);
}

void iniImGui() {
	ImGui::SFML::Init(m_window);
}

void handleAllEvents(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::LControl) {
			KEY_STRG_PRESSED = true;
		}
		else if (event.key.code == sf::Keyboard::L) {
			KEY_L_PRESSED = true;
		}
		if (KEY_STRG_PRESSED && KEY_L_PRESSED) {
			Log::ger().toggleLogWindow();
		}
	}
	else if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::LControl) {
			KEY_STRG_PRESSED = false;
		}
		else if (event.key.code == sf::Keyboard::L) {
			KEY_L_PRESSED = false;
		}
	}
}


void drawLog() {
	if(Log::ger().isLogClosed()) {
		ImGui::Begin("Log", &Log::ger().isLogClosed());

		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (Log::LogEntry& s : Log::ger().getLogs())
		{
			switch(s.label)
			{
				case Log::Label::LOG:
					ImGui::PushStyleColor(ImGuiCol_Text, ImColor(100, 140, 100).Value);
					break;
				case Log::Label::WARNING:
					ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 165, 0).Value);
					break;
				case Log::Label::ERROR:
					ImGui::PushStyleColor(ImGuiCol_Text, ImColor(210, 0, 0).Value);
					break;
				default:
					ImGui::PushStyleColor(ImGuiCol_Text, ImColor(127, 127, 127).Value);
					break;
			}

			ImGui::Text(s.message.c_str());
			ImGui::PopStyleColor();
		}

		ImGui::EndChild();
		ImGui::Separator();

		//ImGui::InputText("input",&m_controller.m_log_input,100);

		if (ImGui::Button("Clear")) {
			Log::ger().clearLog();
		}
		ImGui::End();
	}
}

sf::Time detailsRefreshTime = sf::seconds(0.25);
sf::Time detailsTimeSinceLastRefresh = sf::seconds(0.0);
detailMap details;

void drawDetails(sf::Time ellapsed) {
	detailsTimeSinceLastRefresh += ellapsed;
	if (detailsTimeSinceLastRefresh > detailsRefreshTime) {
		details = m_gamePanel.getDetails();
		detailsTimeSinceLastRefresh = sf::seconds(0.0);
	}
	for(auto& obj : details) {
		ImGui::Begin(obj.first.c_str());
		for (auto& detail : obj.second) {
			std::string text = detail.first + ": " + detail.second;
			ImGui::Text(text.c_str());
		}
		ImGui::End();
	}
}

void startMainLoop() {
	while (m_window.isOpen()) {
		sf::Event event;
		while (m_window.pollEvent(event)) {
			handleAllEvents(event);
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) {
				m_window.close();
			}

			switch (m_controller.getActiveWindow()) {
			case ActiveWindow::MAINMENU:
				m_menu.event(event);
				break;
			case ActiveWindow::GAME:
				m_gamePanel.event(event);
				break;
			default:
				m_menu.event(event);
				break;
			}
		}

		sf::Time ellapsed = m_clock.restart();
		m_window.clear(sf::Color::Black);

		ImGui::SFML::Update(m_window, ellapsed);

		switch (m_controller.getActiveWindow()) {
		case ActiveWindow::MAINMENU:
			m_menu.update(ellapsed);
			m_window.draw(m_menu);
			break;
		case ActiveWindow::GAME:
			m_gamePanel.update(ellapsed);
			m_window.draw(m_gamePanel);
			break;
		default:
			m_menu.update(ellapsed);
			m_window.draw(m_menu);
			break;
		}

		drawLog();
		drawDetails(ellapsed);

		ImGui::SFML::Render(m_window);

		m_window.display();
	}
}


int main() {
	try {
		std::cout << "hallo in main" << std::endl;
		iniImGui();
		iniSettings();
		startMainLoop();
	}
	catch (...) {
		std::cout << "error" << std::endl;
	}

	return 0;
}

