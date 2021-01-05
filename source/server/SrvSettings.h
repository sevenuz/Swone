#ifndef SWONE_SERVER_SRVSETTINGS_H
#define SWONE_SERVER_SRVSETTINGS_H

#include <string>
#include "util/reader/Reader.h"
#include "util/Log.h"
#include "util/Helper.h"

#define SRVSETTINGS_FILE "../res/srvsettings.ini"

// settings property names
#define SRVSETTINGS_DOMAIN "domain"
#define SRVSETTINGS_PORT "port"
#define SRVSETTINGS_TICK_RATE "tick-rate"
#define SRVSETTINGS_RESOURCE_DIRECTORY "resource_directory"

class SrvSettings {
public:
	SrvSettings();
	virtual ~SrvSettings();

	std::string getDomain();
	void setDomain(std::string v);

	unsigned short getPort();
	void setPort(unsigned short v);

	unsigned short getTickRate();
	void setTickRate(unsigned short v);

	std::string getResourceDirectory();
	void setResourceDirectory(std::string s);
protected:
private:
	std::string m_domain = "localhost"; // TODO necessary?
	unsigned short m_port = 61007;
	unsigned short m_tickRate = 20; // updates per second
	std::string m_resource_directory = "../res/";
};

#endif

