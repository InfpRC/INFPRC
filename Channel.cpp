
#include "Channel.hpp"

Channel::Channel(std::string const &name) {
	_name = name;
	_topic = "";
  _key = "";
	_limit = MAX_CLIENT;
	_inviteOnly = false;
}

Channel::~Channel() {}

void Channel::setName(std::string const &name) {
	_name = name;
}

void Channel::setTopic(std::string const &topic) {
	_topic = topic;
}

void Channel::setKey(std::string const &key) {
	_key = key;
}

void Channel::setLimit(size_t limit) {
	_limit = limit;
}

void Channel::setInviteOnly(bool inviteOnly) {
	_inviteOnly = inviteOnly;
}

void Channel::inviteClient(int fd) {
	_invited.insert(fd);
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

std::string const &Channel::getKey() const {
	return _key;
}

size_t Channel::getLimit() const {
	return _limit;
}

bool Channel::getInviteOnly() const {
	return _inviteOnly;
}

size_t Channel::getClientNum() {
	return _clients.size();
}

std::map<int, int> &Channel::getClients() {
  return _clients;
}

std::vector<int> Channel::getClientsFd() {
  std::vector<int> fds;
  for (auto it = _clients.begin(); it != _clients.end(); it++) {
    fds.push_back(it->first);
  }
  return fds;
}

int Channel::isInvited(int fd) {
	if (_invited.find(fd) != _invited.end()) {
		return 1;
	}
	return 0;
}