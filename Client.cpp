
#include "Client.hpp"

Client::Client(int fd) : _fd(fd), _is_passed(false), _is_ping(true) {
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	getsockname(_fd, (struct sockaddr *)&client_addr, &addr_len);
	_ip = inet_ntoa(client_addr.sin_addr);
}

Client::~Client() {}


void Client::setNickname(std::string const &nickname) {
	_nickname = nickname;
}

void Client::setUsername(std::string const &username) {
	_username = username;
}

void Client::setRealname(std::string const &realname) {
	_realname = realname;
}

void Client::setRecvBuf(std::string message) {
	_recv_buffer.append(message);
}

void Client::setSendBuf(std::string message) {
	_send_buffer.append(message);
}

void Client::setPassed(bool flag) {
	_is_passed = flag;
}

void Client::setPing(bool flag) {
	_is_ping = flag;
}

std::string Client::getNickname() const {
	return _nickname;
}

std::string Client::getUsername() const {
	return _username;
}

std::string Client::getIp() const {
	return _ip;
}

std::string Client::getRealname() const {
	return _realname;
}

int Client::getFd() const {
	return _fd;
}

std::string Client::getRecvBuf() const {
	return _recv_buffer.substr(0, _recv_buffer.find("\r\n") + 2);
}

std::string Client::getSendBuf() const {
	return _send_buffer;
}

bool Client::getPassed() const {
	return _is_passed;
}

bool Client::getPing() const {
	return _is_ping;
}

void Client::clearRecvBuf() {
	_recv_buffer = _recv_buffer.substr(_recv_buffer.find("\r\n") + 2, _recv_buffer.size());
}

void Client::clearSendBuf() {
	_send_buffer = _send_buffer.substr(_send_buffer.find("\r\n") + 2, _send_buffer.size());
}

int Client::recvSocket() {
	char buf[BUFFER_SIZE] = {0};
	if (recv(_fd, buf, sizeof(buf), 0) <= 0) {
		return EOF;
	}
	setRecvBuf(buf);
	if (_recv_buffer.find("\r\n") != std::string::npos) {
		return END;
	}
	return CONTINUE;
}

int Client::sendSocket() {
	int size = send(_fd, _send_buffer.c_str(), _send_buffer.size(), 0);
	_send_buffer.erase(0, size);
	if (_send_buffer.size() == 0)
		return true;
	return false;
}