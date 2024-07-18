
#include "Channel.hpp"

Channel::Channel(std::string const &name) {
	_name = name;
}

Channel::~Channel() {}

void Channel::setName(std::string const &name) {
	_name = name;
}

void Channel::setTopic(std::string const &topic) {
	_topic = topic;
}

void Channel::setMode(std::string const &mode) {
	_mode = mode;
}

void Channel::addClient(int fd, int chanops) {
	_clients[fd] = chanops;
}

void Channel::delClient(int fd) {
	_clients.erase(fd);
}

std::string const &Channel::getName() const {
	return _name;
}

std::string const &Channel::getTopic() const {
	return _topic;
}

std::string const &Channel::getMode() const {
	return _mode;
}

size_t Channel::getClientNum() {
	return _clients.size();
}

std::map<int, int> &Channel::getClients() {
	return _clients;
}