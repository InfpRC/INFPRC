
#include "Executer.hpp"

Executer::Executer(Client *clnt) : _clnt(clnt) {
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

void Executer::nickCommand() {
	_clnt->setNickname(getParams(0));
}

void Executer::userCommand() {
	_clnt->setUsername(getParams(0));
	_clnt->setRealname(getParams(3));
	_clnt->setSendBuf(makeSource(SERVER) + " 001 " + _clnt->getNickname() + " :Welcome to the Internet Relay Network " + _clnt->getNickname() + "\r\n");
	_clnt->setSendBuf(makeSource(SERVER) + " 002 " + _clnt->getNickname() + " :Your host is irc.seoul42.com\r\n");
	_clnt->setSendBuf(makeSource(SERVER) + " 003 " + _clnt->getNickname() + " :This server was created Mon Jul 9 2024 at 10:00:00 GMT\r\n");
	_clnt->setSendBuf(makeSource(SERVER) + " 004 " + _clnt->getNickname() + " :irc.seoul42.com 1.0 o o\r\n");
}

void Executer::pingCommand() {
	_clnt->setSendBuf(makeSource(SERVER) + " PONG irc.seoul42.com " + getParams(0) + "\r\n");
}

std::string Executer::makeSource(bool is_clnt) {
	std::string source;

	if (is_clnt) {
		source = ":" + _clnt->getNickname() + "!" + _clnt->getUsername() + "@" + _clnt->getIp();
	} else {
		source = ":irc.seoul42.com";
	}
	return source;
}
