#include <game/Scenery.h>

Scenery::Scenery(std::string path, StringMapMap& setupMap, GameReader& gameReader) {
	m_file_list.push_back(path);
	if(setupMap.count(Reader::DEFAULT_PARAGRAPH)){
		auto& global = setupMap[Reader::DEFAULT_PARAGRAPH];
		if(global.count(S_NAME))
			m_name = global[S_NAME];
		else
			throw std::invalid_argument("Name missing.");
		if(global.count(S_MAP)) {
			std::string mapPath = gameReader.getMapPath(global[S_MAP]);
			m_file_list.push_back(mapPath);
			m_map = gameReader.getMap(mapPath);
		} else
			throw std::invalid_argument("Map-Filename missing.");
	}

	for(auto& m : setupMap) {
		std::string paragraph = m.first;
		if(paragraph == Reader::DEFAULT_PARAGRAPH)
			continue;
		std::string goPath = gameReader.getGameObjectPath(paragraph);
		m_file_list.push_back(goPath);
		StringMapMap goSetupMap = gameReader.getGameObjectParagraphMap(goPath);
		for(int i = 1; setupMap[paragraph].count(std::to_string(i)); i++){
			GameObject* go = new GameObject(goSetupMap);
			go->setPos(Helper::toVector2f(setupMap[paragraph][std::to_string(i)]));
			m_game_objects.push_back(go);
		}
	}

	sortGameObjects();
}

Scenery::~Scenery() {
	for(GameObject* go : m_game_objects)
		delete go;
}

void Scenery::sortGameObjects()
{
	// sort list with z-index
	m_game_objects.sort([](GameObject* a, GameObject* b) -> bool {
		return a->getZindex() < b->getZindex();
	});
}

void Scenery::event(sf::Event& event)
{
	getMap()->event(event);

	for (GameObject* g : getGameObjects()) {
		g->event(event);
	}
}

void Scenery::update(sf::Time ellapsed)
{
	getMap()->update(ellapsed);

	for (GameObject* g : getGameObjects()) {
		g->apply();
		g->update(ellapsed);
	}

	sortGameObjects();
}

void Scenery::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = NULL;

	bool drawMap = true;
	for (GameObject* g : getGameObjects()) {
		if(g->getZindex() >= 0 && drawMap) {
			target.draw(*getMap(), states);
			drawMap = false;
		}
		target.draw(*g, states);
	}
	if(drawMap)
		target.draw(*getMap(), states);
};

Map* Scenery::getMap()
{
	return m_map;
}

const Map* Scenery::getMap() const
{
	return m_map;
}

std::list<GameObject*>& Scenery::getGameObjects()
{
	return m_game_objects;
}

const std::list<GameObject*>& Scenery::getGameObjects() const
{
	return m_game_objects;
}

const std::list<std::string>& Scenery::getFileList() const
{
	return m_file_list;
}

void Scenery::setName(std::string n)
{
	m_name = n;
}

std::string Scenery::getName()
{
	return m_name;
}
