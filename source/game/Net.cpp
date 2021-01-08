#include "Net.h"

Net::Packet::Packet(const unsigned char type)
{
	m_type = type;
	*this << m_type;
}
Net::Packet::Packet() : Net::Packet::Packet(T_VOID)
{}
const void* Net::Packet::onSend(std::size_t& size)
{
	return sf::Packet::onSend(size);
}
void Net::Packet::onReceive(const void* data, std::size_t size)
{
	sf::Packet::onReceive(data, size);
	*this >> m_type;
}
const unsigned char Net::Packet::getType()
{
	return m_type;
}




// Type: std::map<T, K>
template<typename T, typename K>
sf::Packet& Net::operator <<(sf::Packet& packet, const std::map<T, K>& sm)
{
	packet  << (sf::Uint16) sm.size();
	for(auto& s : sm)
		packet << s.first << s.second;
	return packet;
}
template<typename T, typename K>
sf::Packet& Net::operator >>(sf::Packet& packet, std::map<T, K>& sm)
{
	sf::Uint16 length = 0;
	packet >> length;
	for(sf::Uint16 i = 0; i < length; i++) {
		std::string first;
		std::string second;
		packet >> first >> second;
		sm[first] = second;
	}
	return packet;
}

// Type: CreateLobbyRequest
sf::Packet& Net::operator <<(sf::Packet& packet, const CreateLobbyRequest& lr)
{
	return packet << lr.name << lr.password << lr.fileMap;
}
sf::Packet& Net::operator >>(sf::Packet& packet, CreateLobbyRequest& lr)
{
	return packet >> lr.name >> lr.password >> lr.fileMap;
}

// Type: CreateLobbyResponse
sf::Packet& Net::operator <<(sf::Packet& packet, const CreateLobbyResponse& lr)
{
	return packet << lr.fileMap;
}
sf::Packet& Net::operator >>(sf::Packet& packet, CreateLobbyResponse& lr)
{
	return packet >> lr.fileMap;
}

