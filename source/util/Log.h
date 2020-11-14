#ifndef SWONE_UTIL_Log_H
#define SWONE_UTIL_Log_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

typedef std::map<std::string, std::string> stringMap;
typedef std::map<std::string, stringMap> valueDetailMap;
typedef std::map<std::string, sf::Texture*> textureMap;
typedef std::map<std::string, textureMap> textureDetailMap;

/*
 * Logger as Singleton inspired by
 * https://stackoverflow.com/a/1008289
 */
class Log { 
public:
	enum Label : char { Default, Warning, Error };
	struct LogEntry {
		std::string message;
		Label label;
	};
	
	// the getInstance fn Log::ger() ;D
	static Log& ger()
	{
		static Log instance;
		return instance;
	}
	
	void log(std::string msg, Label label = Default)
	{
		m_logs.push_back({msg, label});
		std::cout << msg << std::endl;
	}
	
	template<typename T>
	void log(T msg, Label label = Default)
	{
		std::stringstream ss;
		ss << msg;
		log(ss.str(), label);
	}
	
	std::vector<LogEntry>& getLogs()
	{
		return m_logs;
	}
	
	void clearLog()
	{
		m_logs.clear();
	}
	
	void toggleLogWindow()
	{
		m_log_closed = !m_log_closed;
	}
	
	bool& isLogClosed()
	{
		return m_log_closed;
	}

	// used to set textures that are seldomly updated
	// ignores the refreshment timer
	void detailsPutTexture(sf::Texture* texture,
		std::string key, std::string identifier);
	// used to set values that are seldomly updated
	// ignores the refreshment timer
	void detailsPutValue(std::string value,
		std::string key, std::string identifier);

	// Used to set textures that need to be updatet very often
	void detailsUpdateTexture(sf::Texture* texture,
		std::string key, std::string identifier);
	// Used to set values that need to be updatet very often
	void detailsUpdateValue(std::string value,
		std::string key, std::string identifier);

	// Adds ellapsed time to time since last value refresh
	void updateTime(sf::Time ellapsed);
	// Resets time if greater than refresh time
	void resetTime();

	valueDetailMap getValueMap() const;
	textureDetailMap getTextureMap() const;
	
	Log(Log const&)				= delete;
	void operator=(Log const&)	= delete;
private:
	Log() {}
	std::vector<LogEntry> m_logs;
	bool m_log_closed = false;
	valueDetailMap m_value_map;
	textureDetailMap m_texture_map;
	sf::Time m_refresh_time = sf::seconds(0.25);
	sf::Time m_time_since_refresh = m_refresh_time+sf::seconds(1.0);
};

#endif // SWONE_UTIL_Log_H
