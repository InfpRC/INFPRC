
#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include <vector>
#include <sstream>
#include <string>
#include "DataManager.hpp"

#define SERVER 0
#define CLIENT 1

class DataManager;

class Executor
{
private:
	Client *_clnt;
	std::vector<std::string> _params;
	std::string _command;
	DataManager *_data_manager;

	void changeMode(Channel *chan);
	int option_check(std::string option);
	std::string makeSource(bool is_clnt);

public:
	Executor(Client *clnt, DataManager *data_manager);
	~Executor();
	std::string getCommand();
	std::string getParams(int i);

	void passCommand(std::string password);
	void nickCommand(std::string create_time);
	void userCommand(std::string create_time);
	void pingCommand();
	void pongCommand();
	void quitCommand();
	void joinCommand();
	void partCommand();
	void inviteCommand();
	void topicCommand();
	void kickCommand();
	void modeCommand();
	void privmsgCommand();
	void nonCommand();
};

#endif
