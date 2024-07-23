
#include "Socket.hpp"

Socket::Socket(std::string &port) : _port(strtod(port.c_str(), 0)), _address_size(sizeof(_address)) {
	_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
		throw std::runtime_error("socket() error");
	reusePort();
	bindSock();
	listenSock(10);
	setNonBlock(_fd);
}

Socket::~Socket() {}


int Socket::acceptSock() {
	memset(&_address, 0, sizeof(_address));
	int clnt = accept(_fd, (struct sockaddr *)&_address, &_address_size);
	if (clnt == -1)
		throw std::runtime_error("accept() error");
	setNonBlock(clnt);
	return clnt;
}

void Socket::reusePort() {
	int option = true;
	int optlen = sizeof(option);
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optlen);
}

void Socket::bindSock() {
	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);

	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) == -1)
		throw std::runtime_error("bind() error");
}

void Socket::listenSock(int listen_size) {
	if (listen(_fd, listen_size) == -1)
		throw std::runtime_error("listen() error");
}

void Socket::setNonBlock(int fd) {
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

int Socket::getFd() {
	return _fd;
}
