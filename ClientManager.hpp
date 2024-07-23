#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include <map>
#include <algorithm>
#include "Client.hpp"
#include "Kqueue.hpp"
#include "Executer.hpp"

class Executer;

class ClientManager
{
private:
	std::map<int, Client *> _clients;
	Kqueue *_kq;

public:
	ClientManager(Kqueue *kq);
	~ClientManager();

	void addClient(Client *clnt);
	void delClient(int fd);
	Client *getClient(int fd);
	std::string getNicknameByFd(int fd);
	int getFdByNickname(std::string nickname);
	std::map<int, Client *> &getClients();

	int setClientNickname(Client *clnt, Executer executer);
	void setClientUsername(Client *clnt, Executer executer);
};

#endif