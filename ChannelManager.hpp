#ifndef CHANNELMANAGER_HPP
#define CHANNELMANAGER_HPP

#include <map>
#include <algorithm>
#include "Channel.hpp"
#include "Client.hpp"
#include "Executer.hpp"
#include "Kqueue.hpp"
#include "ClientManager.hpp"

class Executer;


class ChannelManager
{
private:
	std::map<std::string, Channel *> _channels;
	Kqueue *_kq;

public:
	ChannelManager(Kqueue *kq);
	~ChannelManager();

	void addChannel(Channel *channel);
	void delChannel(std::string name);

	Channel *getChannel(std::string name);
	std::map<std::string, Channel *> &getChannels();
	void getChannelList();

	void addClientToChannel(std::string channel, Client *clnt, int chanops);
	void delClientFromChannel(std::string channel, int fd);

	int joinChannel(Client *clnt, Executer executer);
	int partChannel(Client *clnt, Executer executer);
	int inviteChannel(Client *clnt, Executer executer);
	int kickChannel(Client *clnt, Executer executer);
	int sendToChannel(Client *clnt, Executer executer);

	bool isChannelOperator(Channel *chan, Client *clnt);
	bool isChannelMember(Channel *chan, Client *clnt);
};

#endif