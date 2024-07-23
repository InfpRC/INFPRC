
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

class Socket {
	private:
		int _port;
		int _fd;
		struct sockaddr_in _address;
		socklen_t _address_size;

		void reusePort();
		void bindSock();
		void listenSock(int listen_size);
		void setNonBlock(int fd);

	public:
		Socket(std::string &port);
		~Socket();

		int acceptSock();

		int getFd();
};


#endif
