#ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "Kqueue.hpp"
#include "Executor.hpp"
#include <map>

class Client;
class Channel;
class Kqueue;
class Executor;


class DataManager
{
private:
	Kqueue *_kq;
	std::map<int, Client *> _clients;
	std::map<std::string, Channel *> _channels;


public:
	DataManager(Kqueue *kq);
	~DataManager();


	// Clients
	void addClient(Client *clnt);
	void delClient(int fd);
	Client *getClient(int fd);
	std::string getNicknameByFd(int fd);
	int getFdByNickname(std::string nickname);
	std::map<int, Client *> &getClients();


	// Channels
	void addChannel(Channel *channel);
	void delChannel(std::string name);

	Channel *getChannel(std::string name);
	std::map<std::string, Channel *> &getChannels();

	void sendToClient(Client *clnt, std::string message);
	void sendToChannel(Channel *chan, std::string message, int except);

	bool isChannelOperator(Channel *chan, Client *clnt);
	bool isChannelMember(Channel *chan, Client *clnt);

	// Client + Channel
	void addClientToChannel(Client *clnt, Channel *chan, int chanops);
	void delClientFromChannel(Client *clnt, Channel *chan);

	void sendToClientChannels(Client *clnt, std::string message);
};

#endif