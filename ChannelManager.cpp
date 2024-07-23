#include "ChannelManager.hpp"

ChannelManager::ChannelManager(Kqueue *kq): _kq(kq) {}

ChannelManager::~ChannelManager() {}

void ChannelManager::addChannel(Channel *channel) {
	_channels[channel->getName()] = channel;
}

void ChannelManager::delChannel(std::string name) {
	_channels.erase(name);
}

Channel *ChannelManager::getChannel(std::string name) {
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	if (it == _channels.end()) {
		return NULL;
	}
	return it->second;
}

std::map<std::string, Channel *> &ChannelManager::getChannels() {
	return _channels;
}

void ChannelManager::addClientToChannel(std::string channel, Client *clnt, int chanops) {
	_channels[channel]->addClient(clnt->getFd(), chanops);
}

void ChannelManager::delClientFromChannel(std::string channel, int fd) {
	_channels[channel]->delClient(fd);
}

void ChannelManager::getChannelList() {
	std::map<std::string, Channel *>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++) {
		// 출력 형식 여기!
		std::cout << it->first << std::endl;
	}
}

int ChannelManager::joinChannel(Client *clnt, Executer executer)
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

int ChannelManager::partChannel(Client *clnt, Executer executer) {
	std::string channel = executer.getParams(0);
	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		return ERR_NOSUCHCHANNEL;
	}
	chan->delClient(clnt->getFd());
	return SUCCESS;
}

int ChannelManager::inviteChannel(Client *clnt, Executer executer) {
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

int ChannelManager::kickChannel(Client *clnt, Executer executer) {
	std::string channel = executer.getParams(0);
	std::string nickname = executer.getParams(1);
	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		return ERR_NOSUCHCHANNEL;
	}
	
	return SUCCESS;
}

bool ChannelManager::isChannelOperator(Channel *chan, Client *clnt) {
	if (chan->getClients().find(clnt->getFd()) == chan->getClients().end()) {
		return false;
	}
	if (chan->getClients()[clnt->getFd()] == 1) {
		return true;
	}
	return false;
}

bool ChannelManager::isChannelMember(Channel *chan, Client *clnt) {
	if (chan->getClients().find(clnt->getFd()) == chan->getClients().end()) {
		return false;
	}
	return true;
}

