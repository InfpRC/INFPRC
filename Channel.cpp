
#include "Channel.hpp"

Channel::Channel() {}

Channel::~Channel() {}

void Channel::initClient(int fd) {
	insert(std::make_pair(fd, Client(fd)));
}

void Channel::delClient(int fd) {
	erase(fd);
}

Client &Channel::getClient(int fd) {
	return find(fd)->second;
}
