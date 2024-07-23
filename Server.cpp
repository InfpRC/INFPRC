
#include "Server.hpp"

Server::Server(std::string _port, std::string password) : _serv(_port), _password(password), _kq(), _data_manager(&_kq) {
}

Server::~Server() {}

void Server::run() {
	_kq.addEvent(_serv.getFd(), EVFILT_READ);
	_kq.updateEvent();
	while (1) {
		int size = _kq.updateEvent();
		for (int i = 0; i < size; i++) {
			struct kevent event = _kq.getEvent(i);
			if (event.flags & EV_ERROR)
			{
				std::cerr << "EV_ERROR: " << event.data << std::endl;
			}
			if (static_cast<int>(event.ident) == _serv.getFd())
			{
				makeNewConnection();
			}
			else if (event.filter == EVFILT_READ)
			{
				eventReadExec(event);
			}
			else if (event.filter == EVFILT_WRITE)
			{
				eventWriteExec(event);
			}
			// else if (event.filter == EVFILT_TIMER)
			// {
			// 	eventTimerExec(event);
			// }
		}
	}
	close(_serv.getFd());
}

void Server::makeNewConnection() {
	int clnt_sock = _serv.acceptSock();
	Client *clnt = new Client(clnt_sock);
	_data_manager.addClient(clnt);
	_kq.addEvent(clnt_sock, EVFILT_READ);
	// _kq.setTimer(clnt_sock);
	std::cout << "connected client: " << clnt_sock << std::endl;
}

void Server::eventReadExec(struct kevent event) {
	Client *clnt = _data_manager.getClient(event.ident);
	if (clnt != NULL) {
		int result = clnt->recvSocket();
		if (result == EOF) {
			std::cout << "closed client: " << clnt->getFd() << std::endl;
			_kq.delEvent(clnt->getFd(), EVFILT_READ);
			close(clnt->getFd());
			_data_manager.delClient(clnt->getFd());
		}
		else if (result == END) {
			parsing(clnt);
		}
	}
}

void Server::eventWriteExec(struct kevent event) {
	Client *clnt = _data_manager.getClient(event.ident);
	if (clnt != NULL) {
		if (clnt->sendSocket()) {
			_kq.delEvent(clnt->getFd(), EVFILT_WRITE);
			if (clnt->getPassed())
			{
				_kq.addEvent(clnt->getFd(), EVFILT_READ);
			}
			else
			{
				std::cout << "closed client: " << clnt->getFd() << std::endl;
				close(clnt->getFd());
				_data_manager.delClient(clnt->getFd());
			}
		}
	}
}

// void Server::eventTimerExec(struct kevent event)
// {
// 	Client *clnt = _data_manager.getClient(event.ident);
// 	if (clnt != NULL)
// 	{
// 		if (clnt->getPing())
// 		{
// 			clnt->setSendBuf(":irc.seoul42.com PING :ping pong\r\n");
// 			clnt->setPing(false);
// 			_kq.delEvent(clnt->getFd(), EVFILT_READ);
// 			_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
// 		}
// 		else
// 		{
// 			clnt->setSendBuf(":irc.seoul42.com NOTICE " + clnt->getNickname() + " :Incorrect PONG response received\r\n");
// 			clnt->setPassed(false);
// 			_kq.delEvent(clnt->getFd(), EVFILT_READ);
// 			_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
// 		}
// 	}
// }

void Server::parsing(Client *clnt) {
	while (clnt->getRecvBuf().size()) {
		std::cout << clnt->getRecvBuf();
		Executor executor(clnt, &_data_manager);
		if (executor.getCommand() == "PASS") {
			executor.passCommand(_password);
		}
		else if (executor.getCommand() == "NICK") {
			executor.nickCommand();
		}
		else if (executor.getCommand() == "USER") {
			executor.userCommand();
		}
		else if (executor.getCommand() == "PING") {
			executor.pingCommand();
		}
		// else if (executor.getCommand() == "PONG")
		// {
		// 	executor.pongCommand();
		// }
		else if (executor.getCommand() == "QUIT")
		{
			executor.quitCommand();
		}
		else if (executor.getCommand() == "JOIN") {
			executor.joinCommand();
		} /* else if (executor.getCommand() == "PRIVMSG") {
			executor.msgCommand();
		} else if (executor.getCommand() == "PART") {
			executor.partCommand();
		} else if (executor.getCommand() == "KICK") {
			executor.kickCommand();
		} else if (executor.getCommand() == "MODE") {
			executor.modeCommand();
		} else if (executor.getCommand() == "") {
			executor.moreCommand();
		} */
		std::cout << clnt->getSendBuf();
	}
}