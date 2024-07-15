
#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <algorithm>

#include "Socket.hpp"
#include "Client.hpp"
#include "Kqueue.hpp"
#include "Channel.hpp"

class Server
{
private:
	Socket serv;
	Kqueue kq;
	std::map<int, Client *> _clients;
	std::map<std::string, Channel *> _channels;

	void echoService(Client &clnt);
	void channelService(Client &clnt);

public:
	Server(std::string _port, std::string _password);
	~Server();

	void run();
	void addClient(Client *clnt);
	void delClient(int fd);

	void makeNewConnection();

	int getClientFdByNickname(std::string nickname);

	std::string getNicknameByClientFd(int fd);

	void addChannel(Channel *channel);
	void delChannel(std::string name);

	void addClientToChannel(std::string channel, int fd, int chanops);
	void delClientFromChannel(std::string channel, int fd);
	void getChannelList();

	void sendToChannel(Client *sender, std::string const &channel, std::string message);
	void sendToClient(std::string nickname, std::string message);
	void sendToAll(std::string _message);

	void setClientNickname(Client *clnt, std::string message);
	void setClientUsername(Client *clnt, std::string message);
	void joinChannel(Client *clnt, std::string channel);
};

#endif
