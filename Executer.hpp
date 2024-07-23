
#ifndef EXECUTER_HPP
#define EXECUTER_HPP

#include <vector>
#include <sstream>

#include "DataManager.hpp"

#define SERVER 0
#define CLIENT 1


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


class Executer {
	private:
		Client *_clnt;
		DataManager *_data_manager;
		std::string _prefix;
		std::string _command;
		std::vector<std::string> _params;

		std::string makeSource(bool is_clnt);

	public:
		Executer(Client *clnt, DataManager *data_manager);
		~Executer();
		std::string getCommand();
		std::string getParams(int i);


		void nickCommand();
		void userCommand();
		void pingCommand();
		void joinCommand();
		void privmsgCommand();
};

#endif
