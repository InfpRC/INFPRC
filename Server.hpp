
#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <algorithm>

#include "Socket.hpp"
#include "Client.hpp"
#include "Kqueue.hpp"
#include "Channel.hpp"
#include "Executer.hpp"

class Message;

class Server
{
private:
	std::string _password;
	Socket _serv;
	Kqueue _kq;
	
	ChannelsManager _channelsManager;
	ClientsManager _clientsManager;

public:
	Server(std::string port, std::string password);
	~Server();

	void run();

	void makeNewConnection();

	void parsing(Client *clnt);

	////NICK
	//void setClientNickname(Client *clnt, std::string nickname);
	////USER
	//void setClientUsername(Client *clnt, Message message);

	//JOIN
	void joinChannel(Client *clnt, Executer executer);

	// PRIVMSG
	void sendToChannel(Client *sender, std::string const &channel, std::string message);

	void sendToAll(std::string _message);
	void sendToClient(Client *sender, std::string const &receiver, std::string message);
	
	



};

#endif
