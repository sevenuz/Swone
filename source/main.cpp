#include <string>
#include "client/Client.h"
#include "server/Server.h"

static constexpr const char* SERVER_SHORT = "-s";
static constexpr const char* SERVER_LONG = "--server";

int main(int argc, char** argv)
{
	if(argc > 1 && (std::string(argv[1]) == SERVER_SHORT || std::string(argv[1]) == SERVER_LONG)) {
		Server server;
		return server.start();
	} else {
		Client client;
		return client.start();
	}
}

