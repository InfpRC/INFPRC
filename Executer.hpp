
#ifndef EXECUTER_HPP
#define EXECUTER_HPP

#include <vector>
#include <sstream>

#include "ClientsManager.hpp"
#include "ChannelsManager.hpp"

#define SERVER 0
#define CLIENT 1

#define NON 0
#define ONLY 1
#define ALL 2

class ClientsManager;
class ChannelsManager;

class Executer {
private:
	Client *_clnt;
	ClientsManager *_clients_manager;
	ChannelsManager *_channels_manager;
	std::string _command;
	std::vector<std::string> _params;

	std::string makeSource(bool is_clnt);
	std::string makeTrailing(std::string message);

public:
	Executer(Client *clnt, ClientsManager *clients_manager, ChannelsManager *channels_manager);
	~Executer();
	std::string getCommand();
	std::string getParams(int i);

	int passCommand(std::string password);
	int nickCommand();
	int userCommand();
	int pingCommand();
	int pongCommand();
	int quitCommand();
	int joinCommand();
};

#endif
