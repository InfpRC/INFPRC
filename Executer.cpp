
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

int Executer::passCommand(std::string password) {
	int flag = NON;
	try {
		if (_clnt->getPassed()) {
			throw std::logic_error(makeSource(SERVER) + " 462 " + _clnt->getNickname() + " :You may not reregister\r\n");
		} else if (getParams(0) != password) {
			throw std::logic_error(makeSource(SERVER) + " 464 " + _clnt->getNickname() + " :Password incorrect\r\n");
		}
		_clnt->setPassed(true);
	} catch (std::exception &e) {
		_clnt->setSendBuf(e.what());
		flag = ONLY;
	}
	return flag;
}

int Executer::nickCommand() {
	int flag = NON;
	try {
		if (!_clnt->getPassed()) {
			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " PASS :Not enough parameters\r\n");
		} else if (_params.size() != 1) {
			throw std::logic_error(makeSource(SERVER) + " 431 " + _clnt->getNickname() + " :No nickname given\r\n");
		}
		std::string nick = getParams(0);
		if (nick.size() < 1 || nick.size() > 9) {
			throw std::logic_error(makeSource(SERVER) + " 432 " + _clnt->getNickname() + " :Erroneus nickname\r\n");
		}
		std::string not_start = "$:#~&%+0123456789";
		std::string not_contain = " ,*?!@.";
		for (size_t i = 0; i < nick.size(); i++) {
			if (i == 0 && not_start.find(nick[i]) != std::string::npos) {
				throw std::logic_error(makeSource(SERVER) + " 432 " + _clnt->getNickname() + " :Erroneus nickname\r\n");
			} else if (not_contain.find(nick[i]) != std::string::npos) {
				throw std::logic_error(makeSource(SERVER) + " 432 " + _clnt->getNickname() + " :Erroneus nickname\r\n");
			}
		}
		if (_data_manager->getFdByNickname(nick) != -1) {
			throw std::logic_error(makeSource(SERVER) + " 433 " + _clnt->getNickname() + " " + nick + " :Nickname is already in use\r\n");
		}
		// _db->announce(_clnt->getFd(), makeSource(CLIENT) + " " + _clnt->getNickname() + " NICK " + nick + "\r\n");
		flag = ALL;
		_clnt->setNickname(getParams(0));
	} catch (std::exception &e) {
		_clnt->setSendBuf(e.what());
		flag = ONLY;
	}
	return flag;
}

int Executer::userCommand() {
	try {
		if (!_clnt->getPassed()) {
			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " PASS :Not enough parameters\r\n");
		} else if (_params.size() != 4) {
			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " USER :Not enough parameters\r\n");
		} else if (!_clnt->getUsername().empty()) {
			throw std::logic_error(makeSource(SERVER) + " 462 " + _clnt->getNickname() + " :You may not reregister\r\n");
		}
		_clnt->setUsername(getParams(0));
		_clnt->setRealname(getParams(3));
		_clnt->setSendBuf(makeSource(SERVER) + " 001 " + _clnt->getNickname() + " :Welcome to the Internet Relay Network " + _clnt->getNickname() + "\r\n");
		_clnt->setSendBuf(makeSource(SERVER) + " 002 " + _clnt->getNickname() + " :Your host is irc.seoul42.com\r\n");
		_clnt->setSendBuf(makeSource(SERVER) + " 003 " + _clnt->getNickname() + " :This server was created Mon Jul 9 2024 at 10:00:00 GMT\r\n");
		_clnt->setSendBuf(makeSource(SERVER) + " 004 " + _clnt->getNickname() + " :irc.seoul42.com 1.0 o o\r\n");

	} catch (std::exception &e) {
		_clnt->setSendBuf(e.what());
	}
	return ONLY;
}

int Executer::pingCommand() {
	try {
		if (!_clnt->getPassed()) {
			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " PASS :Not enough parameters\r\n");
		} else if (_params.size() != 1) {
			throw std::logic_error(makeSource(SERVER) + " 409 " + _clnt->getNickname() + " :No origin specified\r\n");
		}
		_clnt->setSendBuf(makeSource(SERVER) + " PONG irc.seoul42.com :" + getParams(0) + "\r\n");
	} catch (std::exception &e) {
		_clnt->setSendBuf(e.what());
	}
	return ONLY;
}

// int Executer::pongCommand() {
// 	int flag = NON;
// 	try {
// 		if (!_clnt->getPassed()) {
// 			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " PASS :Not enough parameters\r\n");
// 		} else if (_params.size() != 1) {
// 			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " PONG :Not enough parameters\r\n");
// 		} else if (getParams(0) == "ping pong") {
// 			_clnt->setPing(true);
// 		} else {
// 			throw std::logic_error(makeSource(SERVER) + " NOTICE " + _clnt->getNickname() + " :Incorrect PONG response received\r\n");
// 		}
// 	} catch (std::exception &e) {
// 		_clnt->setSendBuf(e.what());
// 		flag = ONLY;
// 	}
// 	return flag;
// }

int Executer::quitCommand() {
	int flag = ALL;
	_clnt->setSendBuf(makeSource(CLIENT) + " QUIT :Quit: " + getParams(0) + "\r\n");
	_clnt->setPassed(false);
	return flag;
}

int Executer::joinCommand() {
	std::stringstream ss_chan(getParams(0));
	std::stringstream ss_key(getParams(1));
	std::vector<std::string> chans;
	std::vector<std::string> keys;
	std::string token;
	while (std::getline(ss_chan, token, ',')) {
        chans.push_back(token);
    }
	while (std::getline(ss_key, token, ',')) {
        keys.push_back(token);
    }
	try {
		if (!_clnt->getPassed()) {
			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " PASS :Not enough parameters\r\n");
		} else if (_params.empty()) {
			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " JOIN :Not enough parameters\r\n");
		} for (size_t i = 0; i < chans.size(); i++) {
			Channel *chan = _data_manager->getChannel(chans[i].substr(1, chans[i].size()));
			int role = CHAN_MEM;
			if (chan == nullptr) {
				chan = new Channel(chans[i].substr(1, chans[i].size()));
				role = CHAN_OPR;
			} else if (chan->getKey() != keys[i]) {
				throw std::logic_error(makeSource(SERVER) + " 475 " + _clnt->getNickname() + " " + chans[i] + " :Cannot join channel (+k)\r\n");
			} else if (chan->getLimit() > 0 && chan->getClientNum() >= chan->getLimit()) {
				throw std::logic_error(makeSource(SERVER) + " 471 " + _clnt->getNickname() + " " + chans[i] + " :Cannot join channel (+l)\r\n");
			} else if (chan->getInviteOnly()) {
				if (chan->isInvited(_clnt->getFd()) == 0) {
					throw std::logic_error(makeSource(SERVER) + " 473 " + _clnt->getNickname() + " " + chans[i] + " :Cannot join channel (+i)\r\n");
				}
			} else if (chans[i][0] != '#') {
				throw std::logic_error(makeSource(SERVER) + " 476 " + chans[i] + " :Bad Channel Mask\r\n");
			}
			chan->addClient(_clnt->getFd(), role);
			/* join reply message */
			// sendToChannel 사용!
		}
	} catch (const std::exception& e) {
		_clnt->setSendBuf(e.what());
	}
	return ONLY;	
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

std::string Executer::makeTrailing(std::string message) {
	std::string trailing = ":";

	if (message.find(" ") != std::string::npos) {
		return trailing + message;
	} else {
		return message;
	}
}
