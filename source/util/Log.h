#ifndef SWONE_UTIL_Log_H
#define SWONE_UTIL_Log_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>

struct Value {
	virtual void display() const = 0;
	virtual ~Value() = 0;
};

template<typename T>
struct SubV : Value {
	SubV(T v, std::function<void(T)>& display) : m_v(v), m_display(display)
	{}
	void display() const override
	{
		m_display(m_v);
	}
	std::function<void(T)>& m_display;
	T m_v;
};

typedef std::map<std::string, std::unique_ptr<Value>> ValueMap;
typedef std::map<std::string, ValueMap> ValueDetailMap;

/*
 * Logger as Singleton inspired by
 * https://stackoverflow.com/a/1008289
 */
class Log {
public:
	enum Label : char { Default, Warning, Error };
	// TODO add visible state
	struct LogEntry {
		std::string message;
		Label label;
		bool visible;
	};

	// the getInstance fn Log::ger() ;D
	static Log& ger()
	{
		static Log instance;
		return instance;
	}

	void log(std::string msg, Label label = Default, bool visible = true)
	{
		m_logs.push_back({msg, label, visible});
		// TODO remove?
		if(visible)
			std::cout << msg << std::endl;
	}

	template<typename T>
	void log(T msg, Label label = Default, bool visible = true)
	{
		std::stringstream ss;
		ss << msg;
		log(ss.str(), label, visible);
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

	void toggleObjectInspect(std::string identifier);

	void registerStringDisplayFun(std::function<void(std::string)> fun);
	void registerTextureDisplayFun(std::function<void(sf::Texture*)> fun);

	const std::vector<std::string>& getObjectIdentifiers() const;
	const ValueDetailMap& getValueMap() const;
	
	Log(Log const&)				= delete;
	void operator=(Log const&)	= delete;
private:
	Log() {}
	std::vector<LogEntry> m_logs;
	bool m_log_closed = false;
	std::vector<std::string> m_object_identifiers;
	ValueDetailMap m_value_map;
	sf::Time m_refresh_time = sf::seconds(0.25);
	sf::Time m_time_since_refresh = m_refresh_time+sf::seconds(1.0);

	std::function<void(std::string)> m_display_string;
	std::function<void(sf::Texture*)> m_display_texture;
};

#endif // SWONE_UTIL_Log_H
