#include "Net.h"

Net::Packet::Packet(const sf::Uint8 type)
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
const sf::Uint8 Net::Packet::getType()
{
	return m_type;
}

Net::GamePacket::GamePacket(const sf::Uint8 type, const std::string& code) : Net::Packet::Packet(type)
{
	m_stamp = Helper::now();
	m_code = code;
	*this << m_stamp << m_code;
}
Net::GamePacket::GamePacket() : Net::Packet::Packet(T_VOID)
{}
const void* Net::GamePacket::GamePacket::onSend(std::size_t& size)
{
	return Net::Packet::onSend(size);
}
void Net::GamePacket::onReceive(const void* data, std::size_t size)
{
	Net::Packet::onReceive(data, size);
	*this >> m_stamp >> m_code;
}
const Net::Timestamp Net::GamePacket::getTimestamp() const
{
	return m_stamp;
}
const std::string Net::GamePacket::getCode() const
{
	return m_code;
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

// Type: sf::Color
sf::Packet& Net::operator <<(sf::Packet& packet, const sf::Color& sm)
{
	return packet << sm.r << sm.g << sm.b << sm.a;
}
sf::Packet& Net::operator >>(sf::Packet& packet, sf::Color& sm)
{
	return packet >> sm.r >> sm.g >> sm.b >> sm.a;
}

void Net::handleGameFileCheck(sf::TcpSocket& socket, const GameFileCheck& gfc, const std::string& resDir)
{
	GameFileCheckAnswer gfca;
	gfca.sceneryFile = !GameReader::getFileHashes().count(gfc.sceneryFile.second);
	gfca.mapFile = !GameReader::getFileHashes().count(gfc.mapFile.second);
	for(auto& p : gfc.objectFileMap)
		if(!GameReader::getFileHashes().count(p.second))
			gfca.objectFiles.push_back(p.first);
	for(auto& p : gfc.textureFileMap)
		if(!GameReader::getFileHashes().count(p.second))
			gfca.textureFiles.push_back(p.first);

	bool needFiles = gfca.sceneryFile
		|| gfca.mapFile
		|| gfca.objectFiles.size()>0
		|| gfca.textureFiles.size()>0;
	if(needFiles) {
		// trigger file upload
		Net::Packet resPacket(Net::T_FILE_REQUEST);
		resPacket << gfca;
		if (socket.send(resPacket) != sf::Socket::Done) {
			throw Net::Status{Net::C_SEND, "handleTcpCreateLobby: Error while sending T_FILE_REQUEST"};
		}
		Net::receiveMissingFiles(socket, gfc, gfca, resDir);
	}
}

void Net::sendMissingFiles(sf::TcpSocket& socket, GameFileCheck gfc, GameFileCheckAnswer gfca)
{
	auto fn = [&](size_t length, char* data) {
		Packet packet(T_FILE);
		packet << File{length, data};
		if (socket.send(packet) != sf::Socket::Done) {
			throw Status{T_FILE, "FileTransport: failed to send packet"};
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

void Net::receiveMissingFiles(sf::TcpSocket& socket, GameFileCheck gfc, GameFileCheckAnswer gfca, const std::string& resDir)
{
	auto fn = [&](std::string file, std::string expectedHash) {
		Packet packet;
		if(socket.receive(packet) != sf::Socket::Done) {
			throw Status{C_RECEIVE, "FileTransport: failed to receive packet"};
		}
		if(packet.getType() == T_FILE) {
			File f;
			packet >> f;
			Helper::writeFileBytes(file, f.length, f.data);
			delete f.data;
			if(expectedHash != GameReader::getHash(file)) { // adds hash to GameReader::FileHashMap
				throw Status{C_CORRUPT, "FileTransport: File corrupt after receiving"};
			}
		} else {
			throw Status{C_TYPE, "FileTransport: expected File, got " + std::to_string(packet.getType())};
		}
	};

	std::string timestamp = std::to_string(Helper::now());
	auto fname = [&](std::string n) -> std::string {
		std::string basename = n.substr(0, n.find_last_of("#"));
		return Helper::replaceIllegalCharacters(basename, "/\\.", '_') + "#" + timestamp;
	};

	std::string file;
	if(gfca.sceneryFile) {
		file = GameReader::getSceneryPath(resDir, fname(gfc.sceneryFile.first));
		fn(file, gfc.sceneryFile.second);
	} else {
		// if sceneryFile is not uploaded, overwrite the GameFileCheck
		// of the used scenerey with the new one to use right files.
		file = GameReader::getFile(gfc.sceneryFile.second);
	}
	{
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

// Type: Status
sf::Packet& Net::operator <<(sf::Packet& packet, const Status& f)
{
	return packet << f.code << f.message;
}
sf::Packet& Net::operator >>(sf::Packet& packet, Status& f)
{
	return packet >> f.code >> f.message;
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
	return packet << lr.code << lr.password;
}
sf::Packet& Net::operator >>(sf::Packet& packet, JoinLobbyReq& lr)
{
	return packet >> lr.code >> lr.password;
}

// Type: JoinLobbyAck
sf::Packet& Net::operator <<(sf::Packet& packet, const JoinLobbyAck& lr)
{
	return packet << lr.code << lr.fileCheck;
}
sf::Packet& Net::operator >>(sf::Packet& packet, JoinLobbyAck& lr)
{
	return packet >> lr.code >> lr.fileCheck;
}

// Type: LobbyStatus
sf::Packet& Net::operator <<(sf::Packet& packet, const LobbyStatus& lr)
{
	return packet << lr.name << lr.code << lr.hasPassword << lr.playerCount;
}
sf::Packet& Net::operator >>(sf::Packet& packet, LobbyStatus& lr)
{
	return packet >> lr.name >> lr.code >> lr.hasPassword >> lr.playerCount;
}

// Type: LobbyRefresh
sf::Packet& Net::operator <<(sf::Packet& packet, const LobbyRefresh& lr)
{
	return packet << lr.lobbies;
}
sf::Packet& Net::operator >>(sf::Packet& packet, LobbyRefresh& lr)
{
	return packet >> lr.lobbies;
}

sf::Packet& Net::operator <<(sf::Packet& packet, const ChatMessageReq& f)
{
	return packet << f.message;
}
sf::Packet& Net::operator >>(sf::Packet& packet, ChatMessageReq& f)
{
	return packet >> f.message;
}

sf::Packet& Net::operator <<(sf::Packet& packet, const ChatMessageAck& f)
{
	return packet << f.stampCheck;
}
sf::Packet& Net::operator >>(sf::Packet& packet, ChatMessageAck& f)
{
	return packet >> f.stampCheck;
}

sf::Packet& Net::operator <<(sf::Packet& packet, const PlayerConfigReq& f)
{
	return packet << f.identifier << f.selection << f.name << f.color;
}
sf::Packet& Net::operator >>(sf::Packet& packet, PlayerConfigReq& f)
{
	return packet >> f.identifier >> f.selection >> f.name >> f.color;
}

sf::Packet& Net::operator <<(sf::Packet& packet, const PlayerConfigAck& f)
{
	return packet << f.stampCheck << f.identifier << f.selection;
}
sf::Packet& Net::operator >>(sf::Packet& packet, PlayerConfigAck& f)
{
	return packet >> f.stampCheck >> f.identifier >> f.selection;
}

sf::Packet& Net::operator <<(sf::Packet& packet, const GameObjectState& f)
{
	return packet << f.identifier << f.key << f.name << f.visible << f.color << f.zindex << f.extensionMap
		<< f.collidableSolid << f.collidableUnsolid << f.rotatable << f.solid << f.skip
		<< f.x << f.y << f.orient << f.vx << f.vy << f.av;
}
sf::Packet& Net::operator >>(sf::Packet& packet, GameObjectState& f)
{
	return packet >> f.identifier >> f.key >> f.name >> f.visible >> f.color >> f.zindex >> f.extensionMap
		>> f.collidableSolid >> f.collidableUnsolid >> f.rotatable >> f.solid >> f.skip
		>> f.x >> f.y >> f.orient >> f.vx >> f.vy >> f.av;
}

sf::Packet& Net::operator <<(sf::Packet& packet, const GameState& f)
{
	return packet << f.objects << f.players;
}
sf::Packet& Net::operator >>(sf::Packet& packet, GameState& f)
{
	return packet >> f.objects >> f.players;
}

sf::Packet& Net::operator <<(sf::Packet& packet, const PlayerInput& f)
{
	return packet << f.identifier << f.inputs;
}
sf::Packet& Net::operator >>(sf::Packet& packet, PlayerInput& f)
{
	return packet >> f.identifier >> f.inputs;
}

sf::Packet& Net::operator <<(sf::Packet& packet, const GameChat& f)
{
	return packet << f.messages;
}
sf::Packet& Net::operator >>(sf::Packet& packet, GameChat& f)
{
	return packet >> f.messages;
}
