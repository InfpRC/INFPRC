
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <set>
#include "Client.hpp"

#define MAX_CLIENT 10

class Channel
{
private:
	std::string _name;
	std::string _topic;
	std::string _key; // 비밀번호
	size_t _limit; // 최대 인원 수
	bool _inviteOnly; // 초대만 가능

	std::map<int, int> _clients; // fd, operator
	std::set<int> _invited; // 초대된 인원
	Channel();

public:
	Channel(std::string const &name);
	~Channel();

	void setName(std::string const &name);
	void setTopic(std::string const &topic);
	void setKey(std::string const &key);
	void setLimit(size_t limit);
	void setInviteOnly(bool inviteOnly);
	void inviteClient(int fd);


	void addClient(int fd, int chanops);
	void delClient(int fd);

	size_t getClientNum();
	std::string const &getName() const;
	std::string const &getTopic() const;
	std::string const &getKey() const;
	size_t getLimit() const;
	bool getInviteOnly() const;
	std::map<int, int> &getClients();
	
	int isInvited(int fd);
};

#endif
