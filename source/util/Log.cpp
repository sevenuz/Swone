#include "util/Log.h"

void Log::detailsPutTexture(sf::Texture* texture,
	std::string key, std::string identifier) {
	m_texture_map[identifier][key] = texture;
}

void Log::detailsPutValue(std::string value,
	std::string key, std::string identifier) {
	m_value_map[identifier][key] = value;
}

void Log::detailsUpdateTexture(sf::Texture* texture,
	std::string key, std::string identifier) {
	if (m_time_since_refresh < m_refresh_time)
		return;
	m_texture_map[identifier][key] = texture;
}

void Log::detailsUpdateValue(std::string value,
	std::string key, std::string identifier) {
	if (m_time_since_refresh < m_refresh_time)
		return;
	m_value_map[identifier][key] = value;
}

void Log::updateTime(sf::Time ellapsed) {
	m_time_since_refresh += ellapsed;
}

void Log::resetTime() {
	if (m_time_since_refresh > m_refresh_time)
		m_time_since_refresh = sf::seconds(0.0);
}

valueDetailMap Log::getValueMap() const {
	return m_value_map;
}

textureDetailMap Log::getTextureMap() const {
	return m_texture_map;
}
