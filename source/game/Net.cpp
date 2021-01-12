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


void Net::sendMissingFiles(sf::TcpSocket& socket, GameFileCheck gfc, GameFileCheckAnswer gfca)
{
	auto fn = [&](size_t length, char* data) {
		Packet packet(T_FILE);
		packet << File{length, data};
		if (socket.send(packet) != sf::Socket::Done) {
			Log::ger().log("FileTransport: failed to send", Log::Label::Error);
		}
	};
	if(gfca.sceneryFile) {
		Helper::readFileBytes(GameReader::getFile(gfc.sceneryFile.second), fn);
	}
	if(gfca.mapFile) {
		Helper::readFileBytes(GameReader::getFile(gfc.mapFile.second), fn);
	}
	for(std::string name : gfca.objectFiles) {
		Helper::readFileBytes(GameReader::getFile(gfc.objectFileMap[name]), fn);
	}
	for(std::string name : gfca.textureFiles) {
		Helper::readFileBytes(GameReader::getFile(gfc.textureFileMap[name]), fn);
	}
}

void Net::receiveMissingFiles(sf::TcpSocket& socket, GameFileCheck gfc, GameFileCheckAnswer gfca, std::string resDir)
{
	auto fn = [&](std::string file, std::string expectedHash) {
		Packet packet;
		if(socket.receive(packet) != sf::Socket::Done) {
			Log::ger().log("FileTransport: failed to receive " + file, Log::Label::Error);
		}
		if(packet.getType() == T_FILE) {
			Log::ger().log("FileDownload: " + file);
			File f;
			packet >> f;
			Helper::writeFileBytes(file, f.length, f.data);
			delete f.data;
			if(expectedHash != GameReader::getHash(file)) { // adds hash to GameReader::FileHashMap
				Log::ger().log("FileDownload corrupt: " + file, Log::Label::Error); // TODO error handling?
			}
		} else {
			Log::ger().log("FileDownload expected", Log::Label::Error);
			// TODO send error status
		}
	};

	std::string timestamp = std::to_string(Helper::now());
	auto fname = [&](std::string n) -> std::string {
		std::string basename = n.substr(0, n.find_last_of("#"));
		return Helper::replaceIllegalCharacters(basename, "/\\.", '_') + "#" + timestamp;
	};

	if(gfca.sceneryFile) {
		std::string file = GameReader::getSceneryPath(resDir, fname(gfc.sceneryFile.first));
		fn(file, gfc.sceneryFile.second);
		// write GameFileCheck wich is injected to Scenery SetupMap in GameReader
		std::string fstr;
		fstr += "// GameFileCheck (automaticly created)\n";
		fstr += std::string(GFC_SCENERY_FILE) + "\n";
		fstr += gfc.sceneryFile.first + "=" + gfc.sceneryFile.second + "\n";
		fstr += std::string(GFC_MAP_FILE) + "\n";
		fstr += gfc.mapFile.first + "=" + gfc.mapFile.second + "\n";
		fstr += std::string(GFC_TEXTURE_FILE_MAP) + "\n";
		for(auto& p : gfc.textureFileMap)
			fstr += p.first + "=" + p.second + "\n";
		fstr += std::string(GFC_OBJECT_FILE_MAP) + "\n";
		for(auto& p : gfc.objectFileMap)
			fstr += p.first + "=" + p.second + "\n";
		Helper::writeFileBytes(file + GameReader::GFC_FILE_EXTENSION, fstr.length(), fstr.data());
	}
	if(gfca.mapFile) {
		fn(GameReader::getMapPath(resDir, fname(gfc.mapFile.first)), gfc.mapFile.second);
	}
	for(std::string name : gfca.objectFiles) {
		fn(GameReader::getGameObjectPath(resDir, fname(name)), gfc.objectFileMap[name]);
	}
	for(std::string name : gfca.textureFiles) {
		fn(GameReader::getTexturePath(resDir, fname(name)), gfc.textureFileMap[name]);
	}
}

// Type: File
// add chars as Uint8 to avoid \0 termination on char*
sf::Packet& Net::operator <<(sf::Packet& packet, const File& f)
{
	packet << f.length;
	for(size_t i = 0; i<f.length; i++) {
		packet << (sf::Uint8) f.data[i];
	}
	return packet;
}
sf::Packet& Net::operator >>(sf::Packet& packet, File& f)
{
	packet >> f.length;
	f.data = new char[f.length];
	sf::Uint8 c;
	for(size_t i = 0; i<f.length; i++) {
		packet >> c;
		f.data[i] = (char) c;
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
