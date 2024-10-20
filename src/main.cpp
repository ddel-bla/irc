#include "../inc/IRCServer.hpp"

int main() {
	IRCServer server;

	if (server.start()) {
		server.run();
	}

	return 0;
}
