#include <server/SrvSettings.h>

SrvSettings::SrvSettings()
{
	try {
		Log::ger().log("-----SrvSettings-----");
		Reader r(SRVSETTINGS_FILE);
		r.forEach([&](std::string p, std::string k, std::string v){
			Log::ger().log(k + " = " + v);
			if(k==SRVSETTINGS_DOMAIN)
				setDomain(v);
			else if(k==SRVSETTINGS_PORT)
				setPort((ushort)Helper::toLong(v));
			else if(k==SRVSETTINGS_TICK_RATE)
				setTickRate((ushort)Helper::toLong(v));
			else
				Log::ger().log(k + " is not a srvsettings option", Log::Label::Warning);
		});
		Log::ger().log("---------------------");
	} catch(const std::invalid_argument& ia) {
		Log::ger().log(ia.what(), Log::Label::Error);
		Reader::write(SRVSETTINGS_FILE, {{"global", {
			StringPair(SRVSETTINGS_DOMAIN, getDomain()),
			StringPair(SRVSETTINGS_PORT, std::to_string(getPort())),
			StringPair(SRVSETTINGS_TICK_RATE, std::to_string(getTickRate())),
		}}});
		Log::ger().log("Write Default-SrvSettings. Try to restart.");
	}
}

SrvSettings::~SrvSettings() {}

std::string SrvSettings::getDomain()
{
	return m_domain;
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
	m_resource_directory = s;
}
