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

// Type: std::vector<T>
template<typename T>
sf::Packet& Net::operator <<(sf::Packet& packet, const std::vector<T>& sm)
{
	packet  << (sf::Uint16) sm.size();
	for(auto& s : sm)
		packet << s;
	return packet;
}
template<typename T>
sf::Packet& Net::operator >>(sf::Packet& packet, std::vector<T>& sm)
{
	sf::Uint16 length = 0;
	packet >> length;
	for(sf::Uint16 i = 0; i < length; i++) {
		T s;
		packet >> s;
		sm.push_back(s);
	}
	return packet;
}

// Type: std::pair<T, K>
template<typename T, typename K>
sf::Packet& Net::operator <<(sf::Packet& packet, const std::pair<T, K>& s)
{
	packet  << s.first << s.second;
	return packet;
}
template<typename T, typename K>
sf::Packet& Net::operator >>(sf::Packet& packet, std::pair<T, K>& s)
{
	packet >> s.first >> s.second;
	return packet;
}

// Type: std::map<T, K>
template<typename T, typename K>
sf::Packet& Net::operator <<(sf::Packet& packet, const std::map<T, K>& sm)
{
	packet  << (sf::Uint16) sm.size();
	for(auto& s : sm)
		packet << s;
	return packet;
}
template<typename T, typename K>
sf::Packet& Net::operator >>(sf::Packet& packet, std::map<T, K>& sm)
{
	sf::Uint16 length = 0;
	packet >> length;
	for(sf::Uint16 i = 0; i < length; i++) {
		std::pair<T, K> s;
		packet >> s;
		sm[s.first] = s.second;
	}
	return packet;
}

// Type: GameFileCheck
sf::Packet& Net::operator <<(sf::Packet& packet, const GameFileCheck& lr)
{
	return packet << lr.sceneryFile << lr.mapFile << lr.objectFileMap << lr.textureFileMap;
}
sf::Packet& Net::operator >>(sf::Packet& packet, GameFileCheck& lr)
{
	return packet >> lr.sceneryFile >> lr.mapFile >> lr.objectFileMap >> lr.textureFileMap;
}

// Type: CreateLobbyReq
sf::Packet& Net::operator <<(sf::Packet& packet, const CreateLobbyReq& lr)
{
	return packet << lr.name << lr.password << lr.fileCheck;
}
sf::Packet& Net::operator >>(sf::Packet& packet, CreateLobbyReq& lr)
{
	return packet >> lr.name >> lr.password >> lr.fileCheck;
}

// Type: GameFileCheckAnswer
sf::Packet& Net::operator <<(sf::Packet& packet, const GameFileCheckAnswer& lr)
{
	return packet << lr.sceneryFile << lr.mapFile << lr.objectFiles << lr.textureFiles;
}
sf::Packet& Net::operator >>(sf::Packet& packet, GameFileCheckAnswer& lr)
{
	return packet >> lr.sceneryFile >> lr.mapFile >> lr.objectFiles >> lr.textureFiles;
}

// Type: JoinLobbyReq
sf::Packet& Net::operator <<(sf::Packet& packet, const JoinLobbyReq& lr)
{
	return packet << lr.identifier << lr.password;
}
sf::Packet& Net::operator >>(sf::Packet& packet, JoinLobbyReq& lr)
{
	return packet >> lr.identifier >> lr.password;
}

// Type: JoinLobbyAck
sf::Packet& Net::operator <<(sf::Packet& packet, const JoinLobbyAck& lr)
{
	return packet << lr.port << lr.fileCheck;
}
sf::Packet& Net::operator >>(sf::Packet& packet, JoinLobbyAck& lr)
{
	return packet >> lr.port >> lr.fileCheck;
}
