
#include "Channel.hpp"

Channel::Channel(std::string const &name): _name(name), _topic(""), _limit(-1), _invite_only(false), _topic_only(true), _created(time(NULL)) {
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

void Channel::setInviteOnly(bool invite_only) {
	_invite_only = invite_only;
}

void Channel::setTopicOnly(bool topic_only) {
	_topic_only = topic_only;
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

void Channel::addOperator(int fd) {
	_clients[fd] = CHAN_OPR;
}

void Channel::delOperator(int fd) {
	_clients[fd] = CHAN_MEM;
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
	return _invite_only;
}

bool Channel::getTopicOnly() const {
	return _topic_only;
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

std::string Channel::getCreated() {
	std::stringstream ss;
	ss << _created;
	return ss.str();
}

std::string Channel::getTopicCreated() {
	std::stringstream ss;
	ss << _topic_created;
	return ss.str();
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
	if (_invite_only) {
		mode_list.append("i");
	}
	if (_topic_only) {
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