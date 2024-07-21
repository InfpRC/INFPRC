
#include "Server.hpp"

Server::Server(std::string port, std::string password) : _serv(port), _password(password), _kq() {}

Server::~Server() {}

void Server::run() {
	_kq.addEvent(_serv.getFd(), EVFILT_READ);
	_kq.updateEvent();
	while (1) {
		int size = _kq.updateEvent();
		for (int i = 0; i < size; i++) {
			struct kevent event = _kq.getEvent(i);
			if (event.flags & EV_ERROR) {
				std::cerr << "EV_ERROR: " << event.data << std::endl;
			}
			if (static_cast<int>(event.ident) == _serv.getFd()) {
				makeNewConnection();
			} else if (event.filter == EVFILT_READ) {
				eventReadExec(event);
			} else if (event.filter == EVFILT_WRITE) {
				eventWriteExec(event);
			}
		}
	}
	close(_serv.getFd());
}

void Server::makeNewConnection() {
	int clnt_sock = _serv.acceptSock();
	Client *clnt = new Client(clnt_sock);
	_clients_manager.addClient(clnt);
	_kq.addEvent(clnt_sock, EVFILT_READ);
	std::cout << "connected client: " << clnt_sock << std::endl;
}

void Server::eventReadExec(struct kevent event) {
	Client *clnt = _clients_manager.getClient(event.ident);
	if (clnt != NULL) {
		int result = clnt->recvSocket();
		if (result == EOF) {
			std::cout << "closed client: " << clnt->getFd() << std::endl;
			_kq.delEvent(clnt->getFd(), EVFILT_READ);
			close(clnt->getFd());
			_clients_manager.delClient(clnt->getFd());
		} else if (result == END) {
			parsing(clnt);
		}
	}
}

void Server::eventWriteExec(struct kevent event) {
	Client *clnt = _clients_manager.getClient(event.ident);
	if (clnt != NULL) {
		if (clnt->sendSocket()) {
			_kq.delEvent(clnt->getFd(), EVFILT_WRITE);
			if (clnt->getPassed()) {
				_kq.addEvent(clnt->getFd(), EVFILT_READ);
			} else {
				close(clnt->getFd());
				_clients_manager.delClient(clnt->getFd());
			}
		}
	}
}

void Server::parsing(Client *clnt) {
	while (clnt->getRecvBuf().size()) {
		int flag = NON;
		std::cout << clnt->getRecvBuf();
		Executer executer(clnt, &_clients_manager, &_channels_manager);
		if (executer.getCommand() == "PASS") {
			flag = executer.passCommand(_password);
		} else if (executer.getCommand() == "NICK") {
			flag = executer.nickCommand();
		} else if (executer.getCommand() == "USER") {
			flag = executer.userCommand();
		} /* else if (executer.getCommand() == "PING") {
			flag = executer.pingCommand();
		} else if (executer.getCommand() == "JOIN") {
			flag = executer.joinCommand();
		} else if (executer.getCommand() == "PRIVMSG") {
			flag = executer.msgCommand();
		} else if (executer.getCommand() == "PART") {
			flag = executer.partCommand();
		} else if (executer.getCommand() == "QUIT") {
			flag = executer.quitCommand();
		} else if (executer.getCommand() == "KICK") {
			flag = executer.kickCommand();
		} else if (executer.getCommand() == "MODE") {
			flag = executer.modeCommand();
		} else if (executer.getCommand() == "") {
			flag = executer.moreCommand();
		} */
		std::cout << executer.getCommand() << ": " << flag << std::endl;
		if (flag == ONLY) {
			_kq.delEvent(clnt->getFd(), EVFILT_READ);
			_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
		} else if (flag == ALL) {
			_kq.delEvent(clnt->getFd(), EVFILT_READ); // vector로 담을 수 있도록 해야함.
			_kq.addEvent(clnt->getFd(), EVFILT_WRITE); 
		}
	}
}

// void Server::parsing(Client *clnt) {
// 	while (clnt->getRecvBuf().size()) {
// 		std::cout << clnt->getRecvBuf() << std::endl;
// 		Executer executer(clnt);
// 		std::string command = executer.getCommand();
// 		if (command == "NICK") {
// 			_clients_manager.setClientNickname(clnt, executer.getParams(0), _kq);
// 		} else if (command == "USER") {
// 			_clients_manager.setClientUsername(clnt, executer, _kq);
// 		} else if (command == "JOIN") {
// 			//joinChannel(clnt, executer);
// 		} else if (command == "PRIVMSG") {
// 			std::string target = executer.getParams(0);
// 			std::string msg = executer.getParams(1);
// 			if (target[0] == '#') {
// 				sendToChannel(clnt, target.substr(target.find("#") + 1), msg);
// 			} else {
// 				sendToClient(clnt, target, msg);
// 			}
// 		} else {
// 			std::cout << "Unknown command: " << command << std::endl;
// 		}
// 	}
// }


void Server::joinChannel(Client *clnt, Executer executer) {
	std::string channel = executer.getParams(0);
	std::string key = executer.getParams(1);
	Channel *chan = _channels_manager.getChannel(channel);
	if (chan == NULL) {
		Channel *new_chan = new Channel(channel);
		_channels_manager.addChannel(new_chan);
		new_chan->addClient(clnt->getFd(), 1);
		if (key.size() > 0) {
			new_chan->setKey(key);
		}
		
	} else {
		// check mode
		if (chan->getInviteOnly()) {
			if (chan->isInvited(clnt->getFd()) == 0) {
				// send error executer
				clnt->setSendBuf("You are not invited\n");
				_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
				return;
			}
		}

		// check key
		if (chan->getKey() != "" && chan->getKey() != key) {
			// send error executer
			clnt->setSendBuf("Wrong key\n");
			_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
			return;
		}

		// check limit
		if (chan->getLimit() > 0 && chan->getClientNum() >= chan->getLimit()) {
			// send error executer
			clnt->setSendBuf("Channel is full\n");
			_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
			return;
		}
		chan->addClient(clnt->getFd(), 0);
	}
	// send join executer
	std::string join_executer = ":" + clnt->getNickname() + "!" + clnt->getUsername() + "@hostname" + " JOIN " + "#" + channel + "\n";
	clnt->setSendBuf(join_executer);
	_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
}


void Server::sendToChannel(Client *sender, std::string const &channel, std::string executer) {
	std::cout << "channel: " << channel << std::endl;
	std::cout << "executer: " << executer << std::endl;
	Channel *chan = _channels_manager.getChannel(channel);
	if (chan == NULL) {
		sender->setSendBuf("Channel not found\n");
		_kq.addEvent(sender->getFd(), EVFILT_WRITE);
		return;
	}
	std::map<int, int> members = chan->getClients();
	if (members.find(sender->getFd()) == members.end())
	{
		sender->setSendBuf("You are not in the channel\n");
		_kq.addEvent(sender->getFd(), EVFILT_WRITE);
		return;
	}
	std::map<int, int>::const_iterator mem_it = members.begin();
	while (mem_it != members.end()) {
		if (mem_it->first != sender->getFd()) {
			Client *clnt = _clients_manager.getClient(mem_it->first);
			std::cout << "Sending to client: " << clnt->getFd() << std::endl;
			// 여기 호스트네임?
			std::string send_executer = ":" + sender->getNickname() + "!" + sender->getUsername() + "@hostname" + " PRIVMSG " + "#" + channel + " :" + executer + "\n";
			clnt->setSendBuf(send_executer);
			_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
		}
		mem_it++;
	}
}

void Server::sendToClient(Client *sender, std::string const &receiver, std::string executer)
{
	int fd = _clients_manager.getFdByNickname(receiver);
	if (fd == -1)
	{
		sender->setSendBuf("Client not found\n");
		_kq.addEvent(sender->getFd(), EVFILT_WRITE);
		return;
	}
	Client *clnt = _clients_manager.getClient(fd);
	std::string send_executer = ":" + sender->getNickname() + "!" + sender->getUsername() + "@hostname" + " PRIVMSG " + receiver + " :" + executer + "\n";
	clnt->setSendBuf(send_executer);
	_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
}
