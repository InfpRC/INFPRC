
#include "Channel.hpp"

Channel::Channel(std::string const &name): _name(name), _limit(-1), _inviteOnly(false), _created(time(NULL)) {
}

Channel::~Channel() {}

void Channel::setName(std::string const &name) {
	_name = name;
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

void Channel::setTopic(std::string const &topic, int author_fd) {
	_topic = topic;
	_topic_created = time(NULL);
	_topic_author = author_fd;
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
	for (std::map<int, int>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		fds.push_back(it->first);
	}
	return fds;
}

time_t Channel::getCreated() {
	return _created;
}

time_t Channel::getTopicCreated() {
	return _topic_created;
}

int Channel::getTopicAuthor() {
	return _topic_author;
}

int Channel::isInvited(int fd) {
	if (_invited.find(fd) != _invited.end()) {
		return 1;
	}
	return 0;
}

std::string Channel::getModeList() {
	std::string mode_list;
	if (_inviteOnly) {
		mode_list.append("i");
	}
	if (_topic.empty()) {
		mode_list.append("t");
	}
	if (_limit >= 0) {
		mode_list.append("l");
	}
	if (!_key.empty()) {
		mode_list.append("k");
	}
	if (_limit >= 0) {
		mode_list.append(" ");
		mode_list.append(std::to_string(_limit));
	}
	if (!_key.empty()) {
		mode_list.append(" ");
		mode_list.append(_key);
	}
	return mode_list;
}