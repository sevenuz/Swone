#include "util/Log.h"

void Log::detailsPutTexture(sf::Texture* texture,
	std::string key, std::string identifier) {
	m_value_map[identifier][key] = std::unique_ptr<Value>(new SubV<sf::Texture*>(texture, m_display_texture));
}

void Log::detailsPutValue(std::string str,
	std::string key, std::string identifier) {
	m_value_map[identifier][key] = std::unique_ptr<Value>(new SubV<std::string>(str, m_display_string));
}

void Log::detailsUpdateTexture(sf::Texture* texture,
	std::string key, std::string identifier) {
	if (m_time_since_refresh < m_refresh_time)
		return;
	m_value_map[identifier][key] = std::unique_ptr<Value>(new SubV<sf::Texture*>(texture, m_display_texture));
}

void Log::detailsUpdateValue(std::string str,
	std::string key, std::string identifier) {
	if (m_time_since_refresh < m_refresh_time)
		return;
	m_value_map[identifier][key] = std::unique_ptr<Value>(new SubV<std::string>(str, m_display_string));
}

void Log::updateTime(sf::Time ellapsed) {
	m_time_since_refresh += ellapsed;
}

void Log::resetTime() {
	if (m_time_since_refresh > m_refresh_time)
		m_time_since_refresh = sf::seconds(0.0);
}

const std::vector<std::string>& Log::getObjectIdentifiers() const {
	return m_object_identifiers;
}

const ValueDetailMap& Log::getValueMap() const {
	return m_value_map;
}

void Log::toggleObjectInspect(std::string identifier) {
	auto it = std::find(m_object_identifiers.begin(),
		m_object_identifiers.end(), identifier);

	if (it == m_object_identifiers.end()) {
		m_object_identifiers.push_back(identifier);
	}
	else {
		m_object_identifiers.erase(it);
	}
}

void Log::registerStringDisplayFun(std::function<void(std::string)> fun)
{
	m_display_string = fun;
}

void Log::registerTextureDisplayFun(std::function<void(sf::Texture*)> fun)
{
	m_display_texture = fun;
}

Value::~Value() {}
