#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <string>

#include "MainMenu.h"
#include "GameMenu.h"
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
GameMenu gameMenu(controller);
sf::Clock game_clock;

bool key_strg_pressed = false;
bool key_l_pressed = false;
bool object_viewer_activated = false;
bool demo_window_activated = false;

void handleAllEvents(sf::Event& event)
{
	if (event.type == sf::Event::Resized) {
		settings.setWidth(event.size.width);
		settings.setHeight(event.size.height);
	} else if(event.type == sf::Event::KeyPressed) {
		if(event.key.code == sf::Keyboard::LControl) {
			key_strg_pressed = true;
		} else if(event.key.code == sf::Keyboard::L) {
			key_l_pressed = true;
		}
		if(key_strg_pressed && key_l_pressed) {
			Log::ger().toggleLogWindow();
		}
	} else if(event.type == sf::Event::KeyReleased) {
		if(event.key.code == sf::Keyboard::LControl) {
			key_strg_pressed = false;
		} else if(event.key.code == sf::Keyboard::L) {
			key_l_pressed = false;
		}
	}
}

void drawLog()
{
	if(Log::ger().isLogClosed()) {
		ImGui::Begin("Log", &Log::ger().isLogClosed());

		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

		for(Log::LogEntry& s : Log::ger().getLogs()) {
			if(!s.visible)
				continue;
			switch(s.label) {
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

		if(ImGui::Button("Clear")) {
			Log::ger().clearLog();
		}
		ImGui::SameLine();
		if(ImGui::Button("Demo Window")) {
			demo_window_activated = !demo_window_activated;
		}
		ImGui::SameLine();
		if(ImGui::Button("Object Viewer")) {
			object_viewer_activated = !object_viewer_activated;
		}
		ImGui::End();
	}
}

void renderObjectSelector()
{
	const static std::list<GameObject*>& gameObjects = gameMenu.getGameController().getGameObjects();

	if(ImGui::TreeNode("GameObjects")) {
		if(ImGui::TreeNode("all")) {
			int id = 0;
			for(GameObject* g : gameObjects) {
				ImGui::PushID(id);
				ImGui::Text(g->getIdentifier().c_str());
				ImGui::SameLine();
				if(ImGui::SmallButton("Inspect Object")) {
					g->toggleLogging();
					Log::ger().toggleObjectInspect(g->getIdentifier());
				}
				ImGui::PopID();
				id++;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void drawObjectViewer()
{
	ImGui::SetNextWindowSize(ImVec2(550, 350), ImGuiCond_FirstUseEver);
	ImGui::Begin("Object Viewer", NULL, ImGuiWindowFlags_MenuBar);

	static bool objectSelectorOpen = false;

	if(ImGui::BeginMenuBar()) {
		if(ImGui::BeginMenu("View")) {
			ImGui::MenuItem("Inspect", NULL, &objectSelectorOpen);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if(objectSelectorOpen)
		renderObjectSelector();

	const static ValueDetailMap& value_detail_map = Log::ger().getValueMap();
	const static std::vector<std::string>& inspected_obj_ids = Log::ger().getObjectIdentifiers();

	gameMenu.updateLog();
	for(const std::string& id : inspected_obj_ids) {
		bool open = true;
		if(ImGui::CollapsingHeader(id.c_str(), &open)) {
			ImGui::BeginChild((id+"information").c_str(), ImVec2(400, 125), true, ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::Columns(3, "details");
			ImGui::SetColumnWidth(0, 200);
			ImGui::SetColumnWidth(1, 100);
			ImGui::SetColumnWidth(2, 100);
			ImGui::Separator(); ImGui::NextColumn();
			ImGui::Text("x"); ImGui::NextColumn();
			ImGui::Text("y"); ImGui::NextColumn();
			ImGui::Separator();
			ImGui::Text("Current Position:"); ImGui::NextColumn();
			value_detail_map.at(id).at("current_x")->display(); ImGui::NextColumn();
			value_detail_map.at(id).at("current_y")->display(); ImGui::NextColumn();
			ImGui::Separator();
			ImGui::Text("Current Velocity Vector:"); ImGui::NextColumn();
			value_detail_map.at(id).at("vel_x")->display(); ImGui::NextColumn();
			value_detail_map.at(id).at("vel_y")->display(); ImGui::NextColumn();
			ImGui::Columns(1); ImGui::Separator();

			ImGui::Columns(3, "flags");
			ImGui::SetColumnWidth(0, 100);
			ImGui::SetColumnWidth(1, 100);
			ImGui::SetColumnWidth(2, 100);
			ImGui::Separator();
			ImGui::Text("isFalling"); ImGui::NextColumn();
			ImGui::Text("isRising"); ImGui::NextColumn();
			ImGui::Text("isMoving"); ImGui::NextColumn();
			ImGui::Separator();
			value_detail_map.at(id).at("isFalling")->display(); ImGui::NextColumn();
			value_detail_map.at(id).at("isRising")->display(); ImGui::NextColumn();
			value_detail_map.at(id).at("isMoving")->display(); ImGui::NextColumn();
			ImGui::Columns(1); ImGui::Separator();
			ImGui::EndChild();

			ImGui::SameLine();
			value_detail_map.at(id).at("animation")->display(3.0);
		}
		if(!open) {
			gameMenu.getGameObjectById(id)->toggleLogging();
			Log::ger().toggleObjectInspect(id);
		}
	}

	ImGui::End();
}

void startMainLoop()
{
	ImGuiIO& io = ImGui::GetIO();
	while(window.isOpen()) {
		sf::Event event;
		while(window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if(event.type == sf::Event::MouseWheelScrolled && io.WantCaptureMouse)
				continue;
			handleAllEvents(event);
			if(event.type == sf::Event::Closed) {
				window.close();
			}

			switch(controller.getActiveWindow()) {
			case ActiveWindow::MAINMENU:
				menu.event(event);
				break;
			case ActiveWindow::GAME:
				gameMenu.event(event);
				break;
			default:
				menu.event(event);
				break;
			}
		}

		sf::Time ellapsed = game_clock.restart();
		Log::ger().updateTime(ellapsed);
		window.clear(settings.getClearingColor());

		ImGui::SFML::Update(window, ellapsed);

		switch(controller.getActiveWindow()) {
		case ActiveWindow::MAINMENU:
			menu.update(ellapsed);
			window.draw(menu);
			break;
		case ActiveWindow::GAME:
			gameMenu.update(ellapsed);
			gameMenu.drawImgui();
			window.draw(gameMenu);
			break;
		default:
			menu.update(ellapsed);
			window.draw(menu);
			break;
		}

		drawLog();
		if(demo_window_activated) {
			ImGui::ShowDemoWindow();
		}
		if(object_viewer_activated)
			drawObjectViewer();
		Log::ger().resetTime();

		ImGui::SFML::Render(window);

		window.display();
	}
}

void initLogger()
{
	std::function<void(std::string, float)> imguiDisplayString = [](std::string s, float scale) {
		ImGui::Text(s.c_str());
	};

	std::function<void(const sf::Texture*, float)> imguiDisplayTexture = [](const sf::Texture* t, float scale) {
		ImGui::Image(*t);
	};

	std::function<void(const sf::Sprite*, float)> imguiDisplaySprite = [](const sf::Sprite* t, float scale) {
		ImGui::Image(*t);
	};

	std::function<void(const AnimatedSprite*, float)> imguiDisplayAnimatedSprite = [](const AnimatedSprite* t, float scale) {
		Image(*t, scale);
	};

	Log::ger().registerDisplayFun<std::string>(imguiDisplayString);
	Log::ger().registerDisplayFun<const sf::Texture*>(imguiDisplayTexture);
	Log::ger().registerDisplayFun<const sf::Sprite*>(imguiDisplaySprite);
	Log::ger().registerDisplayFun<sf::Texture*>(imguiDisplayTexture);
	Log::ger().registerDisplayFun<sf::Sprite*>(imguiDisplaySprite);
	Log::ger().registerDisplayFun<const AnimatedSprite*>(imguiDisplayAnimatedSprite);
	Log::ger().registerDisplayFun<AnimatedSprite*>(imguiDisplayAnimatedSprite);
}

int main()
{
	ImGui::SFML::Init(window);
	initLogger();
	startMainLoop();

	return 0;
}

