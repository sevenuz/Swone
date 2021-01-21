#ifndef SWONE_CLIENT_ONLINEMENU_H
#define SWONE_CLIENT_ONLINEMENU_H

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Color.hpp>

#include <stack>
#include <vector>
#include <string>
#include <sstream>
#include <thread>

#include "graphics/ParticleSystem.h"
#include "game/GameController.h"
#include "game/GameReader.h"
#include "game/Scenery.h"
#include "game/Net.h"
#include "Handleable.h"
#include "client/GameWindow.h"
#include "client/Settings.h"
#include "client/Controller.h"
#include "util/Log.h"
#include "util/Helper.h"

#define LOBBY_NAME_LENGTH 49
#define LOBBY_PASSWORD_LENGTH 25
#define LOBBY_CODE_LENGTH 7
#define PLAYER_NAME_LENGTH 11

class OnlineMenu : public Handleable {
private:
	// Menu: Initial state with Lobby selection
	// Game: GameState are substates of this
	enum State : char { Menu, Game };
	enum GameState : char { Play, Pause, CharacterSelection };
	State m_state = State::Menu;
	GameState m_gstate = GameState::CharacterSelection;

	ParticleSystem m_ps;
	Controller& m_c;

	GameWindow m_gameWindow;
	GameController m_gc;
	Scenery m_scenery;

	sf::Text m_header;

	std::string m_joinLobbyCode;
	std::string m_joinLobbyPassword;
	std::vector<Net::LobbyStatus> m_lobbies;

	std::string m_selectedScenery;
	char m_lobbyName[LOBBY_NAME_LENGTH];
	char m_lobbyPassword[LOBBY_PASSWORD_LENGTH];
	bool m_lobbyPrivate = false;

	std::string m_selectedPlayer;
	char m_playerName[PLAYER_NAME_LENGTH];
	ImVec4 m_playerColor = ImColor(114, 144, 154, 200);

	std::stack<std::string> m_modalMessageStack;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawJoinWindow();
	void drawCreateWindow();

	void drawCharacterSelection();
	void drawPause();

	void createLobby();
	void joinLobby();
public:
	void sendCreateLobbyReq(Net::CreateLobbyReq clr);
	void sendJoinLobbyReq(Net::JoinLobbyReq jlr);
	void handleJoinLobbyAck(sf::TcpSocket& socket, Net::JoinLobbyAck jlr);
	void sendLobbyRefresh();

	void drawImgui();

	void update(sf::Time ellapsed);
	void event(sf::Event& e);

	OnlineMenu(Controller& c);
	~OnlineMenu();
};

#endif
