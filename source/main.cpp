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

Settings settings;
sf::RenderWindow window(sf::VideoMode(settings.getWidth(), settings.getHeight(), settings.getBitsPerPixel()), "Swone o.O");
Controller controller(settings, window);
MainMenu menu(controller);
GamePanel gamePanel(controller);
sf::Clock game_clock;

bool key_strg_pressed = false;
bool key_l_pressed = false;

void handleAllEvents(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::LControl) {
			key_strg_pressed = true;
		}
		else if (event.key.code == sf::Keyboard::L) {
			key_l_pressed = true;
		}
		if (key_strg_pressed && key_l_pressed) {
			Log::ger().toggleLogWindow();
		}
	}
	else if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::LControl) {
			key_strg_pressed = false;
		}
		else if (event.key.code == sf::Keyboard::L) {
			key_l_pressed = false;
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
				case Log::Label::Default:
					ImGui::PushStyleColor(ImGuiCol_Text, ImColor(100, 140, 100).Value);
					break;
				case Log::Label::Warning:
					ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 165, 0).Value);
					break;
				case Log::Label::Error:
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

		//ImGui::InputText("input",&controller.log_input,100);

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
		details = gamePanel.getDetails();
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
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			handleAllEvents(event);
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			switch (controller.getActiveWindow()) {
			case ActiveWindow::MAINMENU:
				menu.event(event);
				break;
			case ActiveWindow::GAME:
				gamePanel.event(event);
				break;
			default:
				menu.event(event);
				break;
			}
		}

		sf::Time ellapsed = game_clock.restart();
		window.clear(settings.getClearingColor());

		ImGui::SFML::Update(window, ellapsed);

		switch (controller.getActiveWindow()) {
		case ActiveWindow::MAINMENU:
			menu.update(ellapsed);
			window.draw(menu);
			break;
		case ActiveWindow::GAME:
			gamePanel.update(ellapsed);
			window.draw(gamePanel);
			break;
		default:
			menu.update(ellapsed);
			window.draw(menu);
			break;
		}

		drawLog();
		drawDetails(ellapsed);

		ImGui::SFML::Render(window);

		window.display();
	}
}


int main() {
	try {
		std::cout << "hallo in main" << std::endl;
		ImGui::SFML::Init(window);
		startMainLoop();
	}
	catch (...) {
		std::cout << "error" << std::endl;
	}

	return 0;
}

