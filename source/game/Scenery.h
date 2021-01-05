#ifndef SWONE_GAME_SCENERY_H
#define SWONE_GAME_SCENERY_H

#include <list>

#include "Handleable.h"
#include "game/GameReader.h"
#include "game/Map.h"
#include "game/object/GameObject.h"

class Scenery : public Handleable {
public:
	static constexpr const char* S_NAME = "name";
	static constexpr const char* S_MAP = "map";

	Scenery(std::string path, StringMapMap& setupMap, GameReader& gameReader); // TODO remove GameReader Dependency
	virtual ~Scenery();

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void event(sf::Event& e);
	void update(sf::Time ellapsed);

	Map* getMap();
	const Map* getMap() const;
	std::list<GameObject*>& getGameObjects();
	const std::list<GameObject*>& getGameObjects() const;
	const std::list<std::string>& getFileList() const;

	void setName(std::string);
	std::string getName();
private:
	void sortGameObjects();

	std::string m_name = "";

	Map* m_map = NULL;
	std::list<GameObject*> m_game_objects;
	std::list<std::string> m_file_list;
};
#endif

