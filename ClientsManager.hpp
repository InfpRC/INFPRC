#ifndef CLIENTSMANAGER_HPP
#define CLIENTSMANAGER_HPP

#include <map>
#include <algorithm>
#include "Client.hpp"
#include "Kqueue.hpp"
#include "Executer.hpp"

class ClientsManager
{
private:
	std::map<int, Client *> _clients;

public:
	ClientsManager();
	~ClientsManager();

	void addClient(Client *clnt);
	void delClient(int fd);
	Client *getClient(int fd);
	std::string getNicknameByFd(int fd);
	int getFdByNickname(std::string nickname);
	std::map<int, Client *> &getClients();

	void setClientNickname(Client *clnt, std::string nickname, Kqueue &kq);
	void setClientUsername(Client *clnt, Executer executer, Kqueue &kq);
};


#endif