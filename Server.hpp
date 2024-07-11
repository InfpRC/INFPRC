
#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>

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
		void addClient(std::string _id, Client *_clnt);
		void delClient(std::string _id);
		void addChannel(std::string _id, Channel *_channel);
		void delChannel(std::string _id);
		void addClientToChannel(std::string _channel_id, std::string _client_id);
		void delClientFromChannel(std::string _channel_id, std::string _client_id);
		void sendToChannel(std::string _channel_id, std::string _message);
		void sendToClient(std::string _client_id, std::string _message);
		void sendToAll(std::string _message);
		
};

#endif
