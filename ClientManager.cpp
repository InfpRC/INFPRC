#include "ClientManager.hpp"

ClientManager::ClientManager(Kqueue *kq) {
	_kq = kq;
}

ClientManager::~ClientManager() {}

void ClientManager::addClient(Client *clnt) {
	_clients[clnt->getFd()] = clnt;
}

void ClientManager::delClient(int fd) {
	_clients.erase(fd);
}

Client *ClientManager::getClient(int fd) {
	std::map<int, Client *>::const_iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		return it->second;
	}
	return NULL;
}

std::string ClientManager::getNicknameByFd(int fd) {
	std::map<int, Client *>::const_iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		return it->second->getNickname();
	}
	return "";
}

int ClientManager::getFdByNickname(std::string nickname) {
	std::map<int, Client *>::const_iterator it = _clients.begin();
	while (it != _clients.end()) {
		if (it->second->getNickname() == nickname) {
			return it->first;
		}
		it++;
	}
	return -1;
}

int ClientManager::setClientNickname(Client *clnt, Executer executer) {
	std::string nickname = executer.getParams(0);
	std::cout << "nickname: " << nickname << std::endl;
	
	// 중복 검사
	int fd = getFdByNickname(nickname);
	if (fd != -1 && fd != clnt->getFd()) {
		return ERR_NICKNAMEINUSE;
	}
	clnt->setNickname(nickname);
	return SUCCESS;
}


void ClientManager::setClientUsername(Client *clnt, Executer executer) {
	std::string username = executer.getParams(0);
	std::string realname = executer.getParams(3);
	clnt->setUsername(username);
	clnt->setRealname(realname);
}

