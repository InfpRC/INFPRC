#include "DataManager.hpp"

DataManager::DataManager(Kqueue *kq) {
	_kq = kq;
}

DataManager::~DataManager() {}

void DataManager::addClient(Client *clnt) {
	_clients[clnt->getFd()] = clnt;
}

void DataManager::delClient(int fd) {
	_clients.erase(fd);
}

Client *DataManager::getClient(int fd) {
	std::map<int, Client *>::const_iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		return it->second;
	}
	return NULL;
}

std::string DataManager::getNicknameByFd(int fd) {
	std::map<int, Client *>::const_iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		return it->second->getNickname();
	}
	return "";
}

int DataManager::getFdByNickname(std::string nickname) {
	std::map<int, Client *>::const_iterator it = _clients.begin();
	while (it != _clients.end()) {
		if (it->second->getNickname() == nickname) {
			return it->first;
		}
		it++;
	}
	return -1;
}

std::map<int, Client *> &DataManager::getClients() {
	return _clients;
}

int DataManager::setClientNickname(Client *clnt, Executor executor) {
	std::string nickname = executor.getParams(0);
	std::cout << "nickname: " << nickname << std::endl;
	
	// 중복 검사
	int fd = getFdByNickname(nickname);
	if (fd != -1 && fd != clnt->getFd()) {
		return ERR_NICKNAMEINUSE;
	}
	clnt->setNickname(nickname);
	return SUCCESS;
}


void DataManager::setClientUsername(Client *clnt, Executor executor) {
	std::string username = executor.getParams(0);
	std::string realname = executor.getParams(3);
	clnt->setUsername(username);
	clnt->setRealname(realname);
}

void DataManager::addChannel(Channel *channel) {
	_channels[channel->getName()] = channel;
}

void DataManager::delChannel(std::string name) {
	_channels.erase(name);
}

Channel *DataManager::getChannel(std::string name) {
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	if (it == _channels.end()) {
		return NULL;
	}
	return it->second;
}

std::map<std::string, Channel *> &DataManager::getChannels() {
	return _channels;
}

void DataManager::getChannelList() {
	std::map<std::string, Channel *>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++) {
		// 출력 형식 여기!
		std::cout << it->first << std::endl;
	}
}

void DataManager::sendToClient(Client *clnt, std::string message) {
	clnt->setSendBuf(message);
	_kq->addEvent(clnt->getFd(), EVFILT_WRITE);
}

void DataManager::sendToChannel(Channel *chan, std::string message, int except)
{
	std::vector<int> fds = chan->getClientsFd();
	for (size_t i = 0; i < fds.size(); i++) {
		if (except > 0 && fds[i] == except) {
			continue;
		}
		sendToClient(getClient(fds[i]), message);
	}
}

void DataManager::sendToAll(std::string message) {
	for (std::map<int, Client *>::iterator iter = _clients.begin(); iter != _clients.end(); iter++) {
		std::cout << iter->second->getNickname() << std::endl;
		sendToClient(iter->second, message);
	}
}

int DataManager::partChannel(Client *clnt, Executor executor) {
	std::string channel = executor.getParams(0);
	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		return ERR_NOSUCHCHANNEL;
	}
	chan->delClient(clnt->getFd());
	return SUCCESS;
}

int DataManager::inviteChannel(Client *clnt, Executor executor) {
	std::string channel = executor.getParams(0);
	std::string nickname = executor.getParams(1);
	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		return ERR_NOSUCHCHANNEL;
	}
	int fd = clnt->getFd();
	chan->inviteClient(fd);
	return SUCCESS;
}

bool DataManager::isChannelOperator(Channel *chan, Client *clnt) {
	if (chan->getClients().find(clnt->getFd()) == chan->getClients().end()) {
		return false;
	}
	if (chan->getClients()[clnt->getFd()] == CHAN_OPR) {
		return true;
	}
	return false;
}

bool DataManager::isChannelMember(Channel *chan, Client *clnt) {
	if (chan->getClients().find(clnt->getFd()) == chan->getClients().end()) {
		return false;
	}
	return true;
}

void DataManager::addClientToChannel(Client *clnt, Channel *chan, int chanops) {
	chan->addClient(clnt->getFd(), chanops);
	clnt->addChannel(chan->getName());
}

void DataManager::delClientFromChannel(Client *clnt, Channel *chan) {
	chan->delClient(clnt->getFd());
	clnt->delChannel(chan->getName());
	if (chan->getClientNum() == 0) {
		delChannel(chan->getName());
	}
}

void DataManager::sendToClientChannels(Client *clnt, std::string message) {
	std::set<int> fds;
	std::set<std::string> channels = clnt->getJoinedChannels();
	for (std::set<std::string>::iterator it = channels.begin(); it != channels.end(); it++) {
		Channel *chan = getChannel(*it);
		std::vector<int> chan_fds = chan->getClientsFd();
		for (size_t i = 0; i < chan_fds.size(); i++) {
			if (chan_fds[i] == clnt->getFd()) {
				continue;
			}
			fds.insert(chan_fds[i]);
		}
	}
	for (std::set<int>::iterator it = fds.begin(); it != fds.end(); it++) {
		sendToClient(getClient(*it), message);
	}
}
