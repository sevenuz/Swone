#ifndef SWONE_UTIL_Log_H
#define SWONE_UTIL_Log_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

/*
 * Logger as Singleton inspired by
 * https://stackoverflow.com/a/1008289
 */
class Log { 
public:
	enum Label : char { LOG, WARNING, ERROR };
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
	
	void log(std::string msg, Label label = LOG)
	{
		m_logs.push_back({msg, label});
		std::cout << msg << std::endl;
	}
	
	template<typename T>
	void log(T msg, Label label = LOG)
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
	
	Log(Log const&)				= delete;
	void operator=(Log const&)	= delete;
private:
	Log() {}
	std::vector<LogEntry> m_logs;
	bool m_log_closed = false;
};

#endif // SWONE_UTIL_Log_H
