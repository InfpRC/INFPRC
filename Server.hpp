
#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <algorithm>

#include "Socket.hpp"
#include "Client.hpp"
#include "Kqueue.hpp"
#include "Channel.hpp"
#include "Executor.hpp"
#include "DataManager.hpp"

class Message;

class Server
{
private:
	Socket _serv;
	std::string _password;
	Kqueue _kq;

	DataManager _data_manager;

public:
	Server(std::string port, std::string password);
	~Server();

	void run();

	void makeNewConnection();
	void eventReadExec(struct kevent event);
	void eventWriteExec(struct kevent event);
	// void eventTimerExec(struct kevent event);

	void parsing(Client *clnt);

	// JOIN
	void joinChannel(Client *clnt, Executor executor);

	// PRIVMSG
	void sendToChannel(Client *sender, std::string const &channel, std::string message);

	void sendToAll(std::string _message);
	void sendToClient(Client *sender, std::string const &receiver, std::string message);
};

#endif
