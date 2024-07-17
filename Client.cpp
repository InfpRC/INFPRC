
#include "Client.hpp"

Client::Client(int fd)
{
	_fd = fd;
	_nickname = "";
	_username = "";
	_realname = "";
	_recv_buffer = "";
	_send_buffer = "";
}

Client::~Client() {}

void Client::setNickname(std::string const &nickname)
{
	_nickname = nickname;
}

void Client::setUsername(std::string const &username)
{
	_username = username;
}

void Client::setRealname(std::string const &realname)
{
	_realname = realname;
}

std::string const &Client::getNickname() const
{
	return _nickname;
}

std::string const &Client::getUsername() const
{
	return _username;
}

std::string const &Client::getRealname() const
{
	return _realname;
}

int const &Client::getFd() const
{
	return _fd;
}

std::string Client::getRecvBuf() const
{
	return _recv_buffer;
}

std::string Client::getSendBuf() const
{
	return _send_buffer;
}

void Client::setRecvBuf(std::string message)
{
	_recv_buffer = message;
}

void Client::setSendBuf(std::string message)
{
	_send_buffer = message;
}

void Client::clearRecvBuf()
{
	_recv_buffer.clear();
}

void Client::clearSendBuf()
{
	_send_buffer.clear();
}

int Client::recvSocket()
{
	char buf[BUFFER_SIZE] = {0};
	if (recv(_fd, buf, sizeof(buf), 0) <= 0)
	{
		return EOF;
	}
	_recv_buffer.append(buf);
	if (_recv_buffer[_recv_buffer.size() - 1] == '\n')
	{
		return END;
	}
	return CONTINUE;
}

int Client::sendSocket()
{
	int size = send(_fd, _send_buffer.c_str(), _send_buffer.size(), 0);
	_send_buffer.erase(0, size);
	if (_send_buffer.size() == 0)
		return true;
	return false;
}