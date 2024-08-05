
#include "Server.hpp"

Server::Server(std::string _port, std::string password) : _serv(_port), _password(password), _kq(), _data_manager(&_kq), _created(time(NULL)) {}

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
			} else if (event.filter == EVFILT_TIMER) {
				eventTimerExec(event);
			}
		}
	}
	close(_serv.getFd());
}

void Server::makeNewConnection() {
	int clnt_sock = _serv.acceptSock();
	Client *clnt = new Client(clnt_sock);
	_data_manager.addClient(clnt);
	_kq.addEvent(clnt_sock, EVFILT_READ);
	_kq.setTimer(clnt_sock);
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

void Server::eventTimerExec(struct kevent event)
{
	Client *clnt = _data_manager.getClient(event.ident);
	if (clnt != NULL)
	{
		if (clnt->getPing()) {
			clnt->setSendBuf(":irc.seoul42.com PING :ping pong\r\n");
			clnt->setPing(false);
			_kq.delEvent(clnt->getFd(), EVFILT_READ);
			_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
		} else {
			clnt->setSendBuf(":irc.seoul42.com NOTICE " + clnt->getNickname() + " :Incorrect PONG response received\r\n");
			clnt->setPassed(false);
			_kq.delEvent(clnt->getFd(), EVFILT_READ);
			_kq.addEvent(clnt->getFd(), EVFILT_WRITE);
		}
	}
}

void Server::parsing(Client *clnt) {
	while (clnt->getRecvBuf().size()) {
		std::cout << "receive: " << clnt->getRecvBuf();
		Executor executor(clnt, &_data_manager);
		std::string command = executor.getCommand();
		if (command == "PASS") {
			executor.passCommand(_password);
		} else if (command == "NICK") {
			executor.nickCommand(getCreated());
		} else if (command == "USER") {
			executor.userCommand(getCreated());
		} else if (command == "PING") {
			executor.pingCommand();
		} else if (command == "PONG") {
			executor.pongCommand();
		} else if (command == "QUIT") {
			executor.quitCommand();
		} else if (command == "JOIN") {
			executor.joinCommand();
		} else if (command == "PART") {
			executor.partCommand();
		} else if (command == "KICK") {
			executor.kickCommand();
		} else if (command == "MODE") {
			executor.modeCommand();
		} else if (command == "PRIVMSG") {
			executor.privmsgCommand();
		}
		std::cout << clnt->getSendBuf();
	}
}

std::string Server::getCreated() {
    // tm 구조체 포인터 생성, time_t를 변환하여 UTC 시간으로 설정
    struct tm* tm_info = std::localtime(&_created);
    
    // 날짜 및 시간 포맷 설정
    char buffer[80];
    std::strftime(buffer, 80, "%a %b %d %Y at %H:%M:%S KST", tm_info);
    
    // 결과를 std::string으로 반환
    return std::string(buffer);
}
