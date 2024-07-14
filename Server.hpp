
#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <algorithm>

#include "Socket.hpp"
#include "Client.hpp"
#include "Kqueue.hpp"
#include "Channel.hpp"

class Server {
	private:
		Socket serv;
		Kqueue kq;
		std::map<std::string, Client *> clients;
		std::map<std::string, Channel *> channels;

		void echoService(Client &clnt);
		void channelService(Client &clnt);

	public:
		Server(std::string _port, std::string _password);
		~Server();

		void run();
		void addClient(std::string nickname, Client *clnt);
		void delClient(std::string nickname);

		int const &getClientFd(std::string nickname) const;

		void addChannel(std::string name, Channel *channel);
		void delChannel(std::string name);

		void addClientToChannel(std::string channel, int fd, int chanops);
		void delClientFromChannel(std::string channel, int fd);

		void sendToChannel(std::string channel, std::string message);
		void sendToClient(std::string nickname, std::string message);
		void sendToAll(std::string _message);
		
};

#endif
