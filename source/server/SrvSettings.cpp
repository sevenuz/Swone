#include <server/SrvSettings.h>

SrvSettings::SrvSettings()
{
	std::string p = Helper::getSavePath(SRVSETTINGS_FILE);
	try {
		Log::ger().log("-----SrvSettings-----");
		read(p);
		Log::ger().log("---------------------");
	} catch(const std::invalid_argument& ia) {
		Log::ger().log(ia.what(), Log::Label::Error);
		std::filesystem::create_directories(Helper::getSavePath());
		Reader::write(p, {{"global", {
			StringPair(SRVSETTINGS_DOMAIN, getDomain()),
			StringPair(SRVSETTINGS_PORT, std::to_string(getPort())),
			StringPair(SRVSETTINGS_TICK_RATE, std::to_string(getTickRate())),
			StringPair(SRVSETTINGS_RESOURCE_DIRECTORY, getResourceDirectory()),
			StringPair(SRVSETTINGS_LOBBY_MAXIMUM, std::to_string(getLobbyMaximum())),
		}}});
		read(p);
		Log::ger().log("Write Default-SrvSettings.");
	}
}

SrvSettings::~SrvSettings() {}

std::string SrvSettings::getDomain()
{
	return m_domain;
}

void SrvSettings::read(const std::string& s)
{
	Reader r(s);
	r.forEach([&](std::string p, std::string k, std::string v){
		Log::ger().log(k + " = " + v);
		if(k==SRVSETTINGS_DOMAIN)
			setDomain(v);
		else if(k==SRVSETTINGS_PORT)
			setPort((unsigned short)Helper::toLong(v));
		else if(k==SRVSETTINGS_TICK_RATE)
			setTickRate((unsigned short)Helper::toLong(v));
		else if(k==SRVSETTINGS_RESOURCE_DIRECTORY)
			setResourceDirectory(v);
		else if(k==SRVSETTINGS_LOBBY_MAXIMUM)
			setLobbyMaximum(Helper::toInt(v));
		else
			Log::ger().log(k + " is not a srvsettings option", Log::Label::Warning);
	});
}

void SrvSettings::setDomain(std::string v)
{
	m_domain = v;
}

void SrvSettings::setPort(unsigned short v)
{
	m_port = v;
}

unsigned short SrvSettings::getPort()
{
	return m_port;
}

void SrvSettings::setTickRate(unsigned short v)
{
	m_tickRate = v;
}

unsigned short SrvSettings::getTickRate()
{
	return m_tickRate;
}

std::string SrvSettings::getResourceDirectory()
{
	return m_resource_directory;
}

void SrvSettings::setResourceDirectory(std::string s)
{
	m_resource_directory = Helper::getSavePath(s);
	GameReader::initResDir(m_resource_directory);
}

unsigned int SrvSettings::getLobbyMaximum()
{
	return m_lobbyMaximum;
}

void SrvSettings::setLobbyMaximum(unsigned int s)
{
	m_lobbyMaximum = s;
}
