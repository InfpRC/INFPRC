
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
		_data_manager->sendToAll(makeSource(CLIENT) + " NICK " + nick + "\r\n");
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
	_clnt->setSendBuf(makeSource(CLIENT) + " QUIT :Quit: " + getParams(0) + "\r\n");
	_clnt->setPassed(false);
	return ALL;
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
				_data_manager->addChannel(chan);
				role = CHAN_OPR;
			} else if (!chan->getKey().empty()) {
				if (keys.size() <= i || keys[i] != chan->getKey()) {
					throw std::logic_error(makeSource(SERVER) + " 475 " + _clnt->getNickname() + " " + chans[i] + " :Cannot join channel (+k)\r\n");
				}
			} else if (chan->getLimit() > 0 && chan->getClientNum() >= chan->getLimit()) {
				throw std::logic_error(makeSource(SERVER) + " 471 " + _clnt->getNickname() + " " + chans[i] + " :Cannot join channel (+l)\r\n");
			} else if (chan->getInviteOnly()) {
				if (chan->isInvited(_clnt->getFd()) == 0) {
					throw std::logic_error(makeSource(SERVER) + " 473 " + _clnt->getNickname() + " " + chans[i] + " :Cannot join channel (+i)\r\n");
				}
			} else if (chans[i][0] != '#') {
				throw std::logic_error(makeSource(SERVER) + " 476 " + chans[i] + " :Bad Channel Mask\r\n");
			}
			_data_manager->addClientToChannel(_clnt, chan, role);
			/* join reply message */
			// sendToChannel 사용!
			_data_manager->sendToChannel(chan, makeSource(CLIENT) + " JOIN :" + chans[i] + "\r\n");
			
			// join한 클라이언트에게 전송
			if (!chan->getTopic().empty()) {
				// 1. RPL_TOPIC (332): 채널의 주제를 전송합니다. 주제가 없을 경우 전송되지 않을 수 있습니다.
				_data_manager->sendToClient(_clnt, makeSource(SERVER) + " 332 " + _clnt->getNickname() + " " + chans[i] + " :" + chan->getTopic() + "\r\n");
				// 2. RPL_TOPICWHOTIME (333): 주제를 설정한 사용자와 시간을 전송합니다. (선택적)
				_data_manager->sendToClient(_clnt, makeSource(SERVER) + " 333 " + _clnt->getNickname() + " " + chans[i] + /* " " + chan->setBy() + " " + chan->setTime() + */"\r\n");
			}
			
			// RPL_NAMREPLY (353): 채널에 현재 참여하고 있는 클라이언트들의 리스트를 전송합니다.
			std::string chan_clnt_list = makeSource(SERVER) + " 353 " + _clnt->getNickname() + " = " + chans[i] + " :";
			std::vector<int> chan_clnts_fd = chan->getClientsFd();
			for (size_t i = 0; i < chan->getClientNum(); i++) {
				std::string prefix;
				// if (chan->isOperator(chan_clnts_fd[i])) {
				// 	prefix = '@';
				// }
				chan_clnt_list.append(prefix + _data_manager->getNicknameByFd(chan_clnts_fd[i]));
				if (i != chan->getClientNum() - 1) {
					chan_clnt_list.append(" ");
				}
			}
			_data_manager->sendToClient(_clnt, chan_clnt_list + "\r\n");
			// RPL_ENDOFNAMES (366): 클라이언트 리스트의 끝을 알리는 메시지입니다.
			_data_manager->sendToClient(_clnt, makeSource(SERVER) + " 366 " + _clnt->getNickname() + " " + chans[i] + " :End of /NAMES list.\r\n");
			
		}
	} catch (const std::exception& e) {
		_clnt->setSendBuf(e.what());
	}
	return ONLY; //only?
}

int Executer::partCommand() {
	std::stringstream ss_chan(getParams(0));
	std::string reason = getParams(1);
	std::vector<std::string> chans;
	std::string token;
	while (std::getline(ss_chan, token, ',')) {
		chans.push_back(token);
	}
	try {
		if (!_clnt->getPassed()) {
			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " PASS :Not enough parameters\r\n");
		} else if (_params.empty()) {
			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " PART :Not enough parameters\r\n");
		}
		for (size_t i = 0; i < chans.size(); i++) {
			Channel *chan = _data_manager->getChannel(chans[i].substr(1));
			if (chan == nullptr) {
				throw std::logic_error(makeSource(SERVER) + " 403 " + _clnt->getNickname() + " " + chans[i] + " :No such channel\r\n");
			} else if (!_data_manager->isChannelMember(chan, _clnt)) {
				throw std::logic_error(makeSource(SERVER) + " 442 " + _clnt->getNickname() + " " + chans[i] + " :You're not on that channel\r\n");
			}
			_data_manager->sendToChannel(chan, makeSource(CLIENT) + " PART " + chans[i] + " :" + reason + "\r\n");
			chan->delClient(_clnt->getFd());
		}
	} catch (const std::exception& e) {
		_clnt->setSendBuf(e.what());
	}
	return ALL;
}

// 명령어 입력 시 채널은 앞에 # 꼭 붙이기
int Executer::kickCommand() {
	std::string chan_name(getParams(0));
	Channel *chan = _data_manager->getChannel(chan_name.substr(1));
	std::stringstream ss_user(getParams(1));
	std::vector<std::string> users;
	std::string comment(getParams(2));
	std::string token;
	while (std::getline(ss_user, token, ',')) {
		users.push_back(token);
	}
	try {
		if (!_clnt->getPassed()) {
			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " PASS :Not enough parameters\r\n");
		} else if (_params.size() < 2) {
			throw std::logic_error(makeSource(SERVER) + " 461 " + _clnt->getNickname() + " KICK :Not enough parameters\r\n");
		} else if (!chan) {
			throw std::logic_error(makeSource(SERVER) + " 403 " + _clnt->getNickname() + " " + chan_name + " :No such channel\r\n");
		} else if (!_data_manager->isChannelMember(chan, _clnt)) {
			throw std::logic_error(makeSource(SERVER) + " 442 " + _clnt->getNickname() + " " + chan_name + " :You're not on that channel\r\n");
		} else if (!_data_manager->isChannelOperator(chan, _clnt)) {
			throw std::logic_error(makeSource(SERVER) + " 482 " + _clnt->getNickname() + " " + chan_name + " :You're not channel operator\r\n");
		}
		for (size_t i = 0; i < users.size(); i++) {
			if (!_data_manager->isChannelMember(chan, _data_manager->getClient(_data_manager->getFdByNickname(users[i])))) {
				throw std::logic_error(makeSource(SERVER) + " 441 " + _clnt->getNickname() + " " + chan_name + " :They aren't on that channel\r\n");
			}
			std::string kick_message = makeSource(CLIENT) + " KICK " + chan_name + " " + users[i] + " ";
			if (!comment.empty()) {
				kick_message.append(comment);
				kick_message.append("\r\n");
			} else {
				kick_message.append(" :You are kicked\r\n");
			}
			_data_manager->sendToChannel(chan, kick_message);
			chan->delClient(_data_manager->getFdByNickname(users[i]));
		}
	} catch(const std::exception& e) {
		_clnt->setSendBuf(e.what());
	}
	return ALL;
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
