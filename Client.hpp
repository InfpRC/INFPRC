
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 100
#define END 0
#define CONTINUE 1

class Client
{
private:
	int _fd;
	std::string _nickname;
	std::string _username;
	std::string _realname;
	std::string _recv_buffer;
	std::string _send_buffer;

public:
	Client(int _fd);
	~Client();

	void setNickname(std::string const &nickname);
	void setUsername(std::string const &username);
	void setRecvBuf(std::string message);
	void setSendBuf(std::string message);

	int const &getFd() const;
	std::string const &getNickname() const;
	std::string const &getUsername() const;
	std::string getRecvBuf() const;
	std::string getSendBuf() const;

	void clearRecvBuf();
	void clearSendBuf();

	int recvSocket();
	int sendSocket();
	void echoService();
};

#endif