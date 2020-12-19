#include <iostream>
#include <string>
#include "client/Client.h"

static constexpr const char* SERVER_SHORT = "-s";
static constexpr const char* SERVER_LONG = "--server";

int main(int argc, char** argv)
{
	if(argc > 1 && (std::string(argv[1]) == SERVER_SHORT || std::string(argv[1]) == SERVER_LONG)) {
		std::cout << "start server" << std::endl;
		return 0;
	} else {
		Client client;
		client.start();
	}
	
	return 0;
}

