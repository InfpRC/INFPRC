
#include "Executer.hpp"

Executer::Executer(Client *clnt, DataManager *data_manager) 
	: _clnt(clnt), _data_manager(data_manager) {
	std::stringstream ss(clnt->getRecvBuf());
	clnt->clearRecvBuf();
	std::string token;
	ss >> _command;
	while (ss >> token && token[0] != ':') {
		_params.push_back(token);
	}
	if (token[0] == ':') {
		_params.push_back(token.substr(1, token.size()));
		while (ss >> token) {
			_params[_params.size() - 1].append(" ");
			_params[_params.size() - 1].append(token);
		}
	}
}

Executer::~Executer() {}

std::string Executer::getCommand() {
	return _command;
}

std::string Executer::getParams(int i) {
	return _params[i];
}


std::string Executer::makeSource(bool is__clnt) {
	std::string source;

	if (is__clnt) {
		source = ":" + _clnt->getNickname() + "!" + _clnt->getUsername() + "@" + _clnt->getIp();
	} else {
		source = ":irc.seoul42.com";
	}
	return source;
}

void Executer::nickCommand() {
	
	int result = _data_manager->setClientNickname(_clnt, *this);
	if (result == ERR_NICKNAMEINUSE) {
		_clnt->setSendBuf(makeSource(SERVER) + " 433 " + _clnt->getNickname() + " :Nickname is already in use\r\n");
	}
	
}

void Executer::userCommand() {
	_data_manager->setClientUsername(_clnt, *this);
	_clnt->setSendBuf(makeSource(SERVER) + " 001 " + _clnt->getNickname() + " :Welcome to the Internet Relay Network " + _clnt->getNickname() + "\r\n");
	_clnt->setSendBuf(makeSource(SERVER) + " 002 " + _clnt->getNickname() + " :Your host is irc.seoul42.com\r\n");
	_clnt->setSendBuf(makeSource(SERVER) + " 003 " + _clnt->getNickname() + " :This server was created Mon Jul 9 2024 at 10:00:00 GMT\r\n");
	_clnt->setSendBuf(makeSource(SERVER) + " 004 " + _clnt->getNickname() + " :irc.seoul42.com 1.0 o o\r\n");
}

void Executer::pingCommand() {
	_clnt->setSendBuf(makeSource(SERVER) + " PONG irc.seoul42.com " + getParams(0) + "\r\n");
}

void Executer::joinCommand() {
	std::string channel_name = getParams(0);
	int result = _data_manager->joinChannel(_clnt, *this);
	if (result == ERR_INVITEONLYCHAN) {}
	else if (result == ERR_BADCHANNELKEY) {}
	else if (result == ERR_CHANNELISFULL) {}
	else if (result == SUCCESS) {}
	else {}
}

void Executer::privmsgCommand() {
	std::string target = getParams(0);
	std::string msg = getParams(1);
	if (target[0] == '#') {
		_data_manager->sendToChannel
	} else {
		
	}
}
