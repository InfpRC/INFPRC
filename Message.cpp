
#include "Message.hpp"

Message::Message(Client *_clnt) : clnt(_clnt) {
	std::stringstream ss(clnt->getRecvBuf());
	clnt->clearRecvBuf();
	std::string token;
	ss >> command;
	while (ss >> token && token[0] != ':') {
		params.push_back(token);
	}
	if (token[0] == ':') {
		trailing = token.substr(1, token.size());
	}
	while (ss >> token) {
		trailing.append(token);
	}
}

Message::~Message() {}

std::string Message::getCommand() {
	return command;
}

std::string Message::getParams(int i) {
	return params[i];
}

