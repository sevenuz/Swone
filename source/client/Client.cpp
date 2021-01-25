#include "client/Client.h"

Client::Client() :
	window(sf::VideoMode(settings.getWidth(), settings.getHeight(), settings.getBitsPerPixel()), "Swone o.O"),
	controller(settings, window),
	menu(controller),
	localMenu(controller),
	onlineMenu(controller),
	settingsMenu(controller),
	gameWindow(controller)
{}

Client::~Client() {}

void Client::handleAllEvents(sf::Event& event)
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

void Client::drawLog()
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

void Client::renderObjectSelector()
{
	const static std::list<GameObject*>& gameObjects = controller.getGameController().getGameObjects();

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

void Client::drawObjectViewer()
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

	//localMenu.updateLog();
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
			//localMenu.getGameObjectById(id)->toggleLogging();
			Log::ger().toggleObjectInspect(id);
		}
	}

	ImGui::End();
}

void Client::startMainLoop()
{
	ImGuiIO& io = ImGui::GetIO();
	while(window.isOpen()) {
		if(!controller.isRunning())
			stop();
		// TODO Handle evenets in another thread
		// http://www.cplusplus.com/forum/beginner/194347/
		sf::Event event;
		while(window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if(event.type == sf::Event::MouseWheelScrolled && io.WantCaptureMouse)
				continue;
			handleAllEvents(event);
			if(event.type == sf::Event::Closed) {
				stop();
			}

			switch(controller.getState()) {
				case Controller::State::MainMenu:
					menu.event(event);
					break;
				case Controller::State::LocalMenu:
					localMenu.event(event);
					break;
				case Controller::State::OnlineMenu:
					onlineMenu.event(event);
					break;
				case Controller::State::SettingsMenu:
					settingsMenu.event(event);
					break;
				case Controller::State::Game:
					gameWindow.event(event);
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

		switch(controller.getState()) {
			case Controller::State::MainMenu:
				menu.update(ellapsed);
				window.draw(menu);
				break;
			case Controller::State::LocalMenu:
				localMenu.update(ellapsed);
				localMenu.drawImgui();
				window.draw(localMenu);
				break;
			case Controller::State::OnlineMenu:
				onlineMenu.update(ellapsed);
				onlineMenu.drawImgui();
				window.draw(onlineMenu);
				break;
			case Controller::State::SettingsMenu:
				settingsMenu.update(ellapsed);
				settingsMenu.drawImgui();
				window.draw(settingsMenu);
				break;
			case Controller::State::Game:
				gameWindow.update(ellapsed);
				gameWindow.drawImgui();
				window.draw(gameWindow);
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

void Client::initLogger()
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

int Client::start()
{
	std::thread serverThread(&Server::start, &server);

	ImGui::SFML::Init(window);
	initLogger();
	controller.start();
	startMainLoop();

	serverThread.join();
	return 0;
}

void Client::stop()
{
	server.stop();
	window.close();
}

