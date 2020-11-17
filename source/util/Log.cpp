#include "util/Log.h"

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

Value::~Value() {}
