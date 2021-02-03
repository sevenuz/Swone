#ifndef SWONE_UTIL_Log_H
#define SWONE_UTIL_Log_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <any>
#include <typeinfo>

struct Value {
	virtual void display(float scale = 1.0) const = 0;
	virtual ~Value() = 0;
};

template<typename T>
struct SubV : Value {
	SubV(T v, std::function<void(T, float)>& display) : m_v(v), m_display(display)
	{}
	void display(float scale) const override
	{
		try {
			m_display(m_v, scale);
		} catch(...) {
			std::cout << "Problem with displaying " << typeid(T).name() << std::endl;
		}
	}
	T m_v;
	std::function<void(T, float)>& m_display;
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


	template<typename T>
	void detailsPutValue(T value,
		std::string key, std::string identifier)
	{
		try {
			std::function<void(T, float)>& f = std::any_cast<std::function<void(T, float)>&>(
				m_funs.at(typeid(T).name()));
			m_value_map[identifier][key] = std::unique_ptr<Value>(new SubV<T>(value, f));
		} catch(...) {
			std::cout << "Problem with display function of " << typeid(T).name() << std::endl;
		}
	}

	template<typename T>
	void detailsUpdateValue(T value,
		std::string key, std::string identifier)
	{
		if(m_time_since_refresh < m_refresh_time)
			return;
		try{
			std::function<void(T, float)>& f = std::any_cast<std::function<void(T, float)>&>(
				m_funs.at(typeid(T).name()));
			m_value_map[identifier][key] = std::unique_ptr<Value>(new SubV<T>(value, f));
		} catch(...) {
			std::cout << "Problem with display function of " << typeid(T).name() << std::endl;
		}
	}

	// Adds ellapsed time to time since last value refresh
	void updateTime(sf::Time ellapsed);
	// Resets time if greater than refresh time
	void resetTime();

	void toggleObjectInspect(std::string identifier);

	template<typename T>
	void registerDisplayFun(std::function<void(T, float)> fun)
	{
		m_funs[typeid(T).name()] = fun;
	}

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

	std::map<std::string, std::any> m_funs;
};

#endif // SWONE_UTIL_Log_H
