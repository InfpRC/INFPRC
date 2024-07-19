
#include "Server.hpp"

Server::Server(std::string _port, std::string _password) : _serv(_port, _password), _kq() {}

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
				// 새로운 클라 접속
				makeNewConnection();
			} else if (event.filter == EVFILT_READ) {
				// 클라 메시지 수신
				Client *clnt = _clientsManager.getClient(event.ident);
				if (clnt != NULL) {
					int result = clnt->recvSocket();
					if (result == EOF) {
						std::cout << "closed client: " << clnt->getFd() << std::endl;
						_kq.delEvent(clnt->getFd(), EVFILT_READ);
						_clientsManager.delClient(clnt->getFd());
						close(clnt->getFd());
					} else if (result == END) {
						parsing(clnt);
					} else {
						continue;
					}
				}
			} else if (event.filter == EVFILT_WRITE) {
				// 클라 메시지 송신
				Client *clnt = _clientsManager.getClient(event.ident);
				if (clnt != NULL) {
					if (clnt->sendSocket()) {
						_kq.delEvent(clnt->getFd(), EVFILT_WRITE);
					}
				}
			}
		}
	}
	close(_serv.getFd());
}

void Server::makeNewConnection() {
	int clnt_sock = _serv.acceptSock();
	Client *clnt = new Client(clnt_sock);
	_clientsManager.addClient(clnt);
	_kq.addEvent(clnt_sock, EVFILT_READ);
	clnt->setSendBuf("접속 메시지\n");
	_kq.addEvent(clnt_sock, EVFILT_WRITE);
	std::cout << "connected client: " << clnt_sock << std::endl;
}

void Server::parsing(Client *clnt) {
	while (clnt->getRecvBuf().size()) {
		std::cout << clnt->getRecvBuf() << std::endl;
		Executer executer(clnt);
		std::string command = executer.getCommand();
		if (command == "NICK") {
			_clientsManager.setClientNickname(clnt, executer.getParams(0), _kq);
		} else if (command == "USER") {
			_clientsManager.setClientUsername(clnt, executer, _kq);
		} else if (command == "JOIN") {
			//joinChannel(clnt, executer);
		} else if (command == "PRIVMSG") {
			std::string target = executer.getParams(0);
			std::string msg = executer.getParams(1);
			if (target[0] == '#') {
				sendToChannel(clnt, target.substr(target.find("#") + 1), msg);
			} else {
				sendToClient(clnt, target, msg);
			}
		} else {
			std::cout << "Unknown command: " << command << std::endl;
		}
	}
}


void Server::joinChannel(Client *clnt, Executer executer) {
	std::string channel = executer.getParams(0);
	std::string key = executer.getParams(1);
	Channel *chan = _channelsManager.getChannel(channel);
	if (chan == NULL) {
		Channel *new_chan = new Channel(channel);
		_channelsManager.addChannel(new_chan);
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
	Channel *chan = _channelsManager.getChannel(channel);
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
			Client *clnt = _clientsManager.getClient(mem_it->first);
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
	int fd = _clientsManager.getFdByNickname(receiver);
	if (fd == -1)
	{
		sender->setSendBuf("Client not found\n");
		_kq.addEvent(sender->getFd(), EVFILT_WRITE);
		return;
	}
	Client *clnt = _clientsManager.getClient(fd);
	std::string send_executer = ":" + sender->getNickname() + "!" + sender->getUsername() + "@hostname" + " PRIVMSG " + receiver + " :" + executer + "\n";
	clnt->setSendBuf(send_executer);
	_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
}

// void Server::parsing(Client *clnt) {
// 	while (clnt->getRecvBuf().size()) {
// 		std::cout << clnt->getRecvBuf() << std::endl;
// 		Message message(clnt);
// 		if (message.getCommand() == "NICK") {
// 			message.nickCommand();
// 		} else if (message.getCommand() == "USER") {
// 			message.userCommand();
// 		} else if (message.getCommand() == "PING") {
// 			message.pingCommand();
// 		} else if (message.getCommand() == "JOIN") {
// 			message.joinCommand(&database);
// 		} else if (message.getCommand() == "PRIVMSG") {
// 			message.msgCommand();
// 		} else if (message.getCommand() == "PART") {
// 			message.partCommand();
// 		} else if (message.getCommand() == "QUIT") {
// 			message.quitCommand();
// 		} else if (message.getCommand() == "KICK") {
// 			message.kickCommand();
// 		} else if (message.getCommand() == "MODE") {
// 			message.modeCommand();
// 		} else if (message.getCommand() == "") {
// 			message.moreCommand();

// 		}
// 		std::cout << "Nickname: " << clnt->getNickname() << std::endl;
// 		std::cout << "Username: " << clnt->getUsername() << std::endl;
// 		std::cout << "Address: " << clnt->getIp() << std::endl;
// 		std::cout << "Realname: " << clnt->getRealname() << std::endl;
// 		kq.addEvent(clnt->getFd(), EVFILT_WRITE);
// 	}
// }