#include "ClientsManager.hpp"

ClientsManager::ClientsManager() {}

ClientsManager::~ClientsManager() {}

void ClientsManager::addClient(Client *clnt) {
	_clients[clnt->getFd()] = clnt;
}

void ClientsManager::delClient(int fd) {
	delete _clients[fd];
	_clients.erase(fd);
}

Client *ClientsManager::getClient(int fd) {
	std::map<int, Client *>::const_iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		return it->second;
	}
	return NULL;
}

std::string ClientsManager::getNicknameByFd(int fd) {
	std::map<int, Client *>::const_iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		return it->second->getNickname();
	}
	return "";
}

int ClientsManager::getFdByNickname(std::string nickname) {
	std::map<int, Client *>::const_iterator it = _clients.begin();
	while (it != _clients.begin()) {
		if (it->second->getNickname() == nickname) {
			return it->first;
		}
		it++;
	}
	return -1;
}

void ClientsManager::setClientNickname(Client *clnt, std::string nickname, Kqueue &kq) {
	std::cout << "nickname: " << nickname << std::endl;
	
	/* 닉네임 유효성 검사 */

	// 중복 검사
	int fd = getFdByNickname(nickname);
	if (fd == -1 || fd == clnt->getFd() ) {
		clnt->setNickname(nickname);
	} else {
		clnt->setSendBuf("Nickname already exists\n");
		kq.addEvent(clnt->getFd(), EVFILT_WRITE);
	}

}


void ClientsManager::setClientUsername(Client *clnt, Executer executer, Kqueue &kq)
{
	std::string username = executer.getParams(0);
	std::string realname = executer.getParams(3);
	/* username, realname 유효성 검사 */
	clnt->setUsername(username);
	clnt->setRealname(realname);
	clnt->setSendBuf(":irc.seoul42.com 001 " + clnt->getNickname() + " :Welcome to the Internet Relay Network randomuser\n");
	clnt->setSendBuf(":irc.seoul42.com 002 " + clnt->getNickname() + " :Your host is irc.seoul42.com\n");
	clnt->setSendBuf(":irc.seoul42.com 003 " + clnt->getNickname() + " :This server was created Mon Jul 9 2024 at 10:00:00 GMT\n");
	clnt->setSendBuf(":irc.seoul42.com 004 " + clnt->getNickname() + " :irc.seoul42.com 1.0 o o\n");
	kq.addEvent(clnt->getFd(), EVFILT_WRITE);
}

