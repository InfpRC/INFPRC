
#ifndef EXECUTER_HPP
#define EXECUTER_HPP

#include <vector>
#include <sstream>

#include "ClientsManager.hpp"
#include "ChannelsManager.hpp"

#define SERVER 0
#define CLIENT 1

class Executer {
	private:
		Client *_clnt;
		ClientsManager *_clients_manager;
		ChannelsManager *_channels_manager;
		std::string _prefix;
		std::string _command;
		std::vector<std::string> _params;

		std::string makeSource(bool is_clnt);

	public:
		Executer(Client *clnt);
		~Executer();
		std::string getCommand();
		std::string getParams(int i);


		void nickCommand();
		void userCommand();
		void pingCommand();
		void joinCommand();
};

#endif
