
#include "Server.hpp"

Server::Server(std::string _port, std::string _password) : serv(_port, _password), kq() {}

Server::~Server() {}

void Server::run()
{
	kq.addEvent(serv.getFd(), EVFILT_READ);
	kq.updateEvent();
	while (1)
	{
		int size = kq.updateEvent();
		for (int i = 0; i < size; i++)
		{
			struct kevent event = kq.getEvent(i);
			if (event.flags & EV_ERROR)
			{
				std::cerr << "EV_ERROR: " << event.data << std::endl;
			}
			if (static_cast<int>(event.ident) == serv.getFd())
			{
				// 새로운 클라 접속
				makeNewConnection();
			}
			else if (event.filter == EVFILT_READ)
			{
				// 클라 메시지 수신
				Client *clnt = _clients.find(event.ident)->second;
				int result = clnt->recvSocket();
				if (result == EOF)
				{
					std::cout << "closed client: " << clnt->getFd() << std::endl;
					kq.delEvent(clnt->getFd(), EVFILT_READ);
					delClient(clnt->getFd());
					close(clnt->getFd());
				}
				else if (result == END)
				{
					std::string message = clnt->getRecvBuf();
					std::string nickname = clnt->getNickname();
					std::string username = clnt->getUsername();
					if (nickname == "")
					{
						setClientNickname(clnt, message);
					}
					else if (username == "")
					{
						setClientUsername(clnt, message);
					}
					else
					{
						std::string command = message.substr(0, message.find(" "));
						std::string content = message.substr(message.find(" ") + 1);
						content = content.substr(0, content.find("\r\n"));
						if (command == "JOIN")
						{
							joinChannel(clnt, content);
						}
						else if (command == "PART")
						{
							std::string channel = content;

							delClientFromChannel(channel, clnt->getFd());
							send(clnt->getFd(), ("Parted channel: " + channel + "\n").c_str(), 19 + channel.size(), 0);
						}
						else if (command == "PRIVMSG")
						{
							std::string channel = content.substr(0, content.find(" "));
							std::string message = content.substr(content.find(" ") + 1);
							sendToChannel(clnt, channel, message);
						}
					}
					clnt->clearRecvBuf();
				}
				else
				{
					continue;
				}
			}
			else if (event.filter == EVFILT_WRITE)
			{
				// 클라 메시지 송신
				Client *clnt = _clients.find(event.ident)->second;
				if (clnt->sendSocket())
				{
					kq.delEvent(clnt->getFd(), EVFILT_WRITE);
				}
			}
		}
	}
	close(serv.getFd());
}

void Server::makeNewConnection()
{
	int clnt_sock = serv.acceptSock();
	Client *clnt = new Client(clnt_sock);
	addClient(clnt);
	kq.addEvent(clnt_sock, EVFILT_READ);
	send(clnt_sock, "Welcome to the server!\n", 24, 0);
	send(clnt_sock, "Set your Nickname\n", 19, 0);
	std::cout << "connected client: " << clnt_sock << std::endl;
}

void Server::setClientNickname(Client *clnt, std::string message)
{
	std::string command = message.substr(0, message.find(" "));
	std::string content = message.substr(message.find(" ") + 1);
	content = content.substr(0, content.find("\n"));
	if (command == "NICK")
	{
		std::cout << "nickname: " << content << std::endl;
		clnt->setNickname(content);
		send(clnt->getFd(), "Now set your Username\n", 23, 0);
	}
	else
	{
		send(clnt->getFd(), "Set your Nickname first\n", 25, 0);
	}
}

void Server::setClientUsername(Client *clnt, std::string message)
{
	std::string command = message.substr(0, message.find(" "));
	std::string content = message.substr(message.find(" ") + 1);
	content = content.substr(0, content.find("\r\n"));
	if (command == "USER")
	{
		std::cout << "username: " << content << std::endl;
		clnt->setUsername(content);
		send(clnt->getFd(), "Hello, ", 7, 0);
		send(clnt->getFd(), clnt->getNickname().c_str(), clnt->getNickname().size(), 0);
		send(clnt->getFd(), "\n", 1, 0);
	}
	else
	{
		send(clnt->getFd(), "Set your Username\n", 20, 0);
	}
}

void Server::joinChannel(Client *clnt, std::string channel)
{
	if (_channels.find(channel) == _channels.end())
	{
		Channel *chan = new Channel(channel);
		addChannel(chan);
		addClientToChannel(channel, clnt->getFd(), 1);
	}
	else
	{
		addClientToChannel(channel, clnt->getFd(), 0);
	}
	send(clnt->getFd(), ("Joined channel: " + channel + "\n").c_str(), 19 + channel.size(), 0);
}

void Server::addClient(Client *clnt)
{
	_clients.insert(std::pair<int, Client *>(clnt->getFd(), clnt));
}

void Server::delClient(int fd)
{
	Client *clnt = _clients.find(fd)->second;
	_clients.erase(fd);
	delete clnt;
}

void Server::sendToChannel(Client *sender, std::string const &channel, std::string message)
{
	std::cout << "channel: " << channel << std::endl;
	std::cout << "message: " << message << std::endl;
	std::map<std::string, Channel *>::iterator it = _channels.find(channel);
	if (it == _channels.end())
	{
		std::cout << "channel not found" << std::endl;
		return;
	}
	Channel *chan = it->second;
	std::map<int, int> members = chan->getClients();
	std::map<int, int>::const_iterator mem_it = members.begin();
	while (mem_it != members.end())
	{
		if (mem_it->first != sender->getFd())
		{
			Client *clnt = _clients.find(mem_it->first)->second;
			std::cout << "Sending to client: " << clnt->getFd() << std::endl;
			clnt->setSendBuf(message);
			kq.addEvent(clnt->getFd(), EVFILT_WRITE);
		}
		mem_it++;
	}
}

int Server::getClientFdByNickname(std::string nickname)
{
	std::map<int, Client *>::const_iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second->getNickname() == nickname)
		{
			return it->first;
		}
		it++;
	}
	return -1;
}

std::string Server::getNicknameByClientFd(int fd)
{
	std::map<int, Client *>::const_iterator it = _clients.find(fd);
	if (it != _clients.end())
	{
		return it->second->getNickname();
	}
	return "";
}

void Server::getChannelList()
{
	std::map<std::string, Channel *>::const_iterator it = _channels.begin();
	while (it != _channels.end())
	{
		std::cout << it->first << std::endl;
		it++;
	}
}

void Server::addChannel(Channel *channel)
{
	std::cout << "addChannel! " << channel->getName() << std::endl;
	_channels.insert(std::pair<std::string, Channel *>(channel->getName(), channel));
	getChannelList();
}

void Server::delChannel(std::string name)
{
	Channel *channel = _channels.find(name)->second;
	delete channel;
	_channels.erase(name);
}

void Server::addClientToChannel(std::string channel, int fd, int chanops)
{
	Channel *chan = _channels.find(channel)->second;
	chan->addClient(fd, chanops);
}

void Server::delClientFromChannel(std::string channel, int fd)
{
	Channel *chan = _channels.find(channel)->second;
	chan->delClient(fd);
	if (chan->getClientNum() == 0)
	{
		delChannel(channel);
	};
}

// void Server::echoService(Client &clnt) {
// 	clnt.setSendBuf(clnt.getRecvBuf());
// 	clnt.clearRecvBuf();
// 	kq.addEvent(clnt.getFd(), EVFILT_WRITE);
// }

// void Server::_channelService(Client &clnt) {
// 	//std::string message = clnt.getRecvBuf();
// 	//Channel::iterator user = channel.begin();
// 	//while (user != channel.end()) {
// 	//	if (user->first == clnt.getFd()) {
// 	//		user++;
// 	//		continue ;
// 	//	}
// 	//	user->second.setSendBuf(clnt.getRecvBuf());
// 	//	kq.addEvent(user->first, EVFILT_WRITE);
// 	//	user++;
// 	//}
// 	//clnt.clearRecvBuf();
// 	//kq.addEvent(clnt.getFd(), EVFILT_WRITE);
// }
