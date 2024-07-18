
#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <algorithm>

#include "Socket.hpp"
#include "Client.hpp"
#include "Kqueue.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "ChannelsManager.hpp"
#include "ClientsManager.hpp"

class Message;

class Server
{
private:
	Socket _serv;
	Kqueue _kq;
	
	ChannelsManager _channelsManager;
	ClientsManager _clientsManager;

	void echoService(Client &clnt);
	void channelService(Client &clnt);

public:
	Server(std::string _port, std::string _password);
	~Server();

	void run();

	void makeNewConnection();

	void parsing(Client *clnt);

	////NICK
	//void setClientNickname(Client *clnt, std::string nickname);
	////USER
	//void setClientUsername(Client *clnt, Message message);

	//JOIN
	void joinChannel(Client *clnt, Message message);

	// PRIVMSG
	void sendToChannel(Client *sender, std::string const &channel, std::string message);

	void sendToAll(std::string _message);
	void sendToClient(Client *sender, std::string const &receiver, std::string message);
	
	



};

#endif
