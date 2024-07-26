
#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include <vector>
#include <sstream>

#include "DataManager.hpp"

#define SERVER 0
#define CLIENT 1

#define CHAN_MEM 0
#define CHAN_OPR 1

// Error Code
#define SUCCESS 0
// NICK Error
#define ERR_NONICKNAMEGIVEN 431
#define ERR_NICKNAMEINUSE 433

// JOIN Error
#define ERR_NEEDMOREPARAMS 461
#define ERR_INVITEONLYCHAN 473
#define ERR_BADCHANNELKEY 475
#define ERR_CHANNELISFULL 471

#define ERR_NOSUCHCHANNEL 403

// PRIVMSG Error
#define ERR_CANNOTSENDTOCHAN 404
#define NON 0
#define ONLY 1
#define ALL 2

class DataManager;

class Executor
{
private:
	Client *_clnt;
	std::vector<std::string> _params;
	std::string _command;
	DataManager *_data_manager;

	std::string makeSource(bool is_clnt);

public:
	Executor(Client *clnt, DataManager *data_manager);
	~Executor();
	std::string getCommand();
	std::string getParams(int i);

	void passCommand(std::string password);
	void nickCommand();
	void userCommand();
	void pingCommand();
	void pongCommand();
	void quitCommand();
	void joinCommand();
	void partCommand();
	void kickCommand();
	void modeCommand();
};

#endif
