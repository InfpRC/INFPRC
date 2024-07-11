
#include "Server.hpp"

int main(int ac, char **av) {
	try {
		if (ac != 3) {
			std::string error = av[0];
			throw std::runtime_error("Usage : " + error + " <port> <password>");
		}
		Server server(av[1], av[2]);
		server.run();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}