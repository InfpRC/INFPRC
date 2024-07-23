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

int DataManager::setClientNickname(Client *clnt, Executer executer) {
	std::string nickname = executer.getParams(0);
	std::cout << "nickname: " << nickname << std::endl;
	
	// 중복 검사
	int fd = getFdByNickname(nickname);
	if (fd != -1 && fd != clnt->getFd()) {
		return ERR_NICKNAMEINUSE;
	}
	clnt->setNickname(nickname);
	return SUCCESS;
}


void DataManager::setClientUsername(Client *clnt, Executer executer) {
	std::string username = executer.getParams(0);
	std::string realname = executer.getParams(3);
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

int DataManager::joinChannel(Client *clnt, Executer executer)
{
	std::string channel = executer.getParams(0);
	std::string key = executer.getParams(1);
	Channel *chan = getChannel(channel);

	// 없는 채널일 경우 새로 생성
	if (chan == NULL) {
		Channel *new_chan = new Channel(channel);
		addChannel(new_chan);
		// 관리자로 추가
		new_chan->addClient(clnt->getFd(), 1);
		// 비밀번호 있으면 설정
		if (key.size() > 0) {
			new_chan->setKey(key);
		}

	} else {

		// 초대 전용 채널일 경우
		if (chan->getInviteOnly()) {
			// 초대된 클라이언트인지 확인
			if (chan->isInvited(clnt->getFd()) == 0) {
				return ERR_INVITEONLYCHAN;
			}
		}

		// 비밀번호 확인
		if (chan->getKey() != "" && chan->getKey() != key) {
			return ERR_BADCHANNELKEY;
		}

		// 인원 제한 확인
		if (chan->getLimit() > 0 && chan->getClientNum() >= chan->getLimit()) {
			return ERR_CHANNELISFULL;
		}
		chan->addClient(clnt->getFd(), 0);
	}
		return SUCCESS;
}

int DataManager::partChannel(Client *clnt, Executer executer) {
	std::string channel = executer.getParams(0);
	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		return ERR_NOSUCHCHANNEL;
	}
	chan->delClient(clnt->getFd());
	return SUCCESS;
}

int DataManager::inviteChannel(Client *clnt, Executer executer) {
	std::string channel = executer.getParams(0);
	std::string nickname = executer.getParams(1);
	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		return ERR_NOSUCHCHANNEL;
	}
	int fd = clnt->getFd();
	chan->inviteClient(fd);
	return SUCCESS;
}

int DataManager::kickChannel(Client *clnt, Executer executer) {
	std::string channel = executer.getParams(0);
	std::string nickname = executer.getParams(1);
	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		return ERR_NOSUCHCHANNEL;
	}
	
	return SUCCESS;
}

int DataManager::sendToChannel(Client *clnt, Executer executer) {
  std::string channel = executer.getParams(0).substr(1);
  std::string message = executer.getParams(1);

  Channel *chan = getChannel(channel);
  if (chan == NULL) {
  }
}

bool DataManager::isChannelOperator(Channel *chan, Client *clnt) {
	if (chan->getClients().find(clnt->getFd()) == chan->getClients().end()) {
		return false;
	}
	if (chan->getClients()[clnt->getFd()] == 1) {
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


