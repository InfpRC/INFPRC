
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>


#define BUFFER_SIZE 100
#define END 0
#define CONTINUE 1

class Client
{
private:
	int _fd;
	bool _is_passed;
	std::string _nickname;
	std::string _username;
	std::string _realname;
	std::string _ip;

	std::string _recv_buffer;
	std::string _send_buffer;

public:
	Client(int _fd);
	~Client();

	void setNickname(std::string const &nickname);
	void setUsername(std::string const &username);
	void setRealname(std::string const &realname);
	void setRecvBuf(std::string message);
	void setSendBuf(std::string message);
	void setPassed();

	int getFd() const;
	std::string getNickname() const;
	std::string getUsername() const;
	std::string getIp() const;
	std::string getRealname() const;
	std::string getRecvBuf() const;
	std::string getSendBuf() const;
	bool getPassed() const;

	void clearRecvBuf();
	void clearSendBuf();

	int recvSocket();
	int sendSocket();
};

#endif