
#include "Socket.hpp"

Socket::Socket(std::string &_port, std::string &_password) : port(strtod(_port.c_str(), 0)), password(_password), address_size(sizeof(address)) {
	fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		throw std::runtime_error("socket() error");
	reusePort();
	bindSock();
	listenSock(10);
	setNonBlock(fd);
}

Socket::~Socket() {}


int Socket::acceptSock() {
	memset(&address, 0, sizeof(address));
	int clnt = accept(fd, (struct sockaddr *)&address, &address_size);
	if (clnt == -1)
		throw std::runtime_error("accept() error");
	setNonBlock(clnt);
	return clnt;
}

void Socket::reusePort() {
	int option = true;
	int optlen = sizeof(option);
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optlen);
}

void Socket::bindSock() {
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	if (bind(fd, (struct sockaddr *)&address, sizeof(address)) == -1)
		throw std::runtime_error("bind() error");
}

void Socket::listenSock(int listen_size) {
	if (listen(fd, listen_size) == -1)
		throw std::runtime_error("listen() error");
}

void Socket::setNonBlock(int fd) {
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

int Socket::getFd() {
	return fd;
}
