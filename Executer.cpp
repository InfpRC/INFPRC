
#include "Executer.hpp"

Executer::Executer(Client *_clnt) : clnt(_clnt) {
	std::stringstream ss(clnt->getRecvBuf());
	clnt->clearRecvBuf();
	std::string token;
	ss >> command;
	while (ss >> token && token[0] != ':') {
		params.push_back(token);
	}
	if (token[0] == ':') {
		params.push_back(token.substr(1, token.size()));
		while (ss >> token) {
			params[params.size() - 1].append(" ");
			params[params.size() - 1].append(token);
		}
	}
}

Executer::~Executer() {}

std::string Executer::getCommand() {
	return command;
}

std::string Executer::getParams(int i) {
	return params[i];
}

void Executer::nickCommand() {
	clnt->setNickname(getParams(0));
}

void Executer::userCommand() {
	clnt->setUsername(getParams(0));
	clnt->setRealname(getParams(3));
	clnt->setSendBuf(makeSource(SERVER) + " 001 " + clnt->getNickname() + " :Welcome to the Internet Relay Network " + clnt->getNickname() + "\r\n");
	clnt->setSendBuf(makeSource(SERVER) + " 002 " + clnt->getNickname() + " :Your host is irc.seoul42.com\r\n");
	clnt->setSendBuf(makeSource(SERVER) + " 003 " + clnt->getNickname() + " :This server was created Mon Jul 9 2024 at 10:00:00 GMT\r\n");
	clnt->setSendBuf(makeSource(SERVER) + " 004 " + clnt->getNickname() + " :irc.seoul42.com 1.0 o o\r\n");
}

void Executer::pingCommand() {
	clnt->setSendBuf(makeSource(SERVER) + " PONG irc.seoul42.com " + getParams(0) + "\r\n");
}

std::string Executer::makeSource(bool is_clnt) {
	std::string source;

	if (is_clnt) {
		source = ":" + clnt->getNickname() + "!" + clnt->getUsername() + "@" + clnt->getIp();
	} else {
		source = ":irc.seoul42.com";
	}
	return source;
}
