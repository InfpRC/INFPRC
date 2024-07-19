#ifndef CHANNELSMANAGER_HPP
#define CHANNELSMANAGER_HPP

#include <map>
#include <algorithm>
#include "Channel.hpp"
#include "Client.hpp"

class ChannelsManager
{
private:
	std::map<std::string, Channel *> _channels;

public:
	ChannelsManager();
	~ChannelsManager();

	void addChannel(Channel *channel);
	void delChannel(std::string name);

	Channel *getChannel(std::string name);
	std::map<std::string, Channel *> &getChannels();
	void getChannelList();

	void addClientToChannel(std::string channel, Client *clnt, int chanops);
	void delClientFromChannel(std::string channel, int fd);

	//void sendToChannel(Client *sender, std::string const &channel, std::string message);
};

#endif