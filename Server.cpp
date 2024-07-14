
#include "Server.hpp"

Server::Server(std::string _port, std::string _password) : serv(_port, _password), kq() {}

Server::~Server() {}

void Server::run() {
	kq.addEvent(serv.getFd(), EVFILT_READ);
	kq.updateEvent();
	while (1) {
		int size = kq.updateEvent();
		for (int i = 0; i < size; i++) {
			struct kevent event = kq.getEvent(i);
			if (event.flags & EV_ERROR) {
				std::cerr << "EV_ERROR: " << event.data << std::endl;
			}
			if (static_cast<int>(event.ident) == serv.getFd()) {
				int clnt_sock = serv.acceptSock();
				// 클라 접속
				kq.addEvent(clnt_sock, EVFILT_READ);
				send(clnt_sock, "Welcome to the server!\nEnterYourNickname", 23, 0);

				//std::cout << "connected client: " << clnt_sock << std::endl;
			} else if (event.filter == EVFILT_READ) {
				// 클라 메시지 수신
				//Client &clnt = channel.getClient(event.ident);
				//int clnt_fd = clnt.getFd();
				//int result = clnt.recvSocket();
				//if (result == EOF) {
				//	std::cout << "closed client: " << clnt_fd << std::endl;
				//	channel.delClient(clnt_fd);
				//	close(clnt_fd);
				//} else if (result == END) {
				//	//echoService(clnt);
				//	channelService(clnt);
				//} else {
				//	continue ;
				//}
			} else if (event.filter == EVFILT_WRITE) {
				// 클라 메시지 송신
				//Client &clnt = channel.getClient(event.ident);
				//if (clnt.sendSocket()) {
				//	kq.delEvent(clnt.getFd(), EVFILT_WRITE);
				//}
			}
		}
	}
	close(serv.getFd());
}

void Server::addClient(std::string nickname, Client *clnt) {
	clients.insert(std::pair<std::string, Client *>(nickname, clnt));
}

void Server::delClient(std::string nickname) {
	Client *clnt = clients.find(nickname)->second;
	clients.erase(nickname);
	delete clnt;
}

int const &Server::getClientFd(std::string nickname) const {
	Client *clnt = clients.find(nickname)->second;
	return clnt->getFd();
}

void Server::addChannel(std::string name, Channel *channel) {
	channels.insert(std::pair<std::string, Channel *>(name, channel));
}

void Server::delChannel(std::string name) {
	Channel *channel = channels.find(name)->second;
	channels.erase(name);
	delete channel;
}

void Server::addClientToChannel(std::string channel, int fd, int chanops) {
	Channel *chan = channels.find(channel)->second;
	chan->addClient(fd, chanops);
}

void Server::delClientFromChannel(std::string channel, int fd) {
	Channel *chan = channels.find(channel)->second;
	chan->delClient(fd);
}

void Server::echoService(Client &clnt) {
	clnt.setSendBuf(clnt.getRecvBuf());
	clnt.clearRecvBuf();
	kq.addEvent(clnt.getFd(), EVFILT_WRITE);
}


void Server::channelService(Client &clnt) {
	//std::string message = clnt.getRecvBuf();
	//Channel::iterator user = channel.begin();
	//while (user != channel.end()) {
	//	if (user->first == clnt.getFd()) {
	//		user++;
	//		continue ;
	//	}
	//	user->second.setSendBuf(clnt.getRecvBuf());
	//	kq.addEvent(user->first, EVFILT_WRITE);
	//	user++;
	//}
	//clnt.clearRecvBuf();
	//kq.addEvent(clnt.getFd(), EVFILT_WRITE);
}
