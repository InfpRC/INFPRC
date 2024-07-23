#ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "Kqueue.hpp"
#include "Executer.hpp"
#include <map>

class Client;
class Channel;
class Kqueue;
class Executer;

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

	int setClientNickname(Client *clnt, Executer executer);
	void setClientUsername(Client *clnt, Executer executer);



	// Channels
	void addChannel(Channel *channel);
	void delChannel(std::string name);

	Channel *getChannel(std::string name);
	std::map<std::string, Channel *> &getChannels();
	void getChannelList();

	int joinChannel(Client *clnt, Executer executer);
	int partChannel(Client *clnt, Executer executer);
	int inviteChannel(Client *clnt, Executer executer);
	int kickChannel(Client *clnt, Executer executer);
	void sendToClient(Client *clnt, std::string message);
	void sendToChannel(Channel *chan, std::string message);
	void sendToAll(std::string message);

	bool isChannelOperator(Channel *chan, Client *clnt);
	bool isChannelMember(Channel *chan, Client *clnt);
};

#endif