#include "ChannelsManager.hpp"

ChannelsManager::ChannelsManager() {}

ChannelsManager::~ChannelsManager() {}

void ChannelsManager::addChannel(Channel *channel) {
	_channels[channel->getName()] = channel;
}

void ChannelsManager::delChannel(std::string name) {
	_channels.erase(name);
}

Channel *ChannelsManager::getChannel(std::string name) {
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	if (it == _channels.end()) {
		return NULL;
	}
	return it->second;
}

std::map<std::string, Channel *> &ChannelsManager::getChannels() {
	return _channels;
}

void ChannelsManager::addClientToChannel(std::string channel, Client *clnt, int chanops) {
	_channels[channel]->addClient(clnt->getFd(), chanops);
}

void ChannelsManager::delClientFromChannel(std::string channel, int fd) {
	_channels[channel]->delClient(fd);
}

void ChannelsManager::getChannelList() {
	std::map<std::string, Channel *>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++) {
		// 출력 형식 여기!
		std::cout << it->first << std::endl;
	}
}

//void ChannelsManager::sendToChannel(Client *sender, std::string const &channel, std::string message) {
//	std::map<int, int> clients = _channels[channel]->getClients();
//	std::map<int, int>::iterator it;
//	for (it = clients.begin(); it != clients.end(); it++) {
//		if (it->first != sender->getFd()) {
//			//
//		}
//	}
//}
