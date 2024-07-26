
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <set>
#include <vector>
#include "Client.hpp"


class Channel
{
private:
	std::string _name;
	std::string _topic;
	std::string _key; // 비밀번호
	size_t _limit;	  // 최대 인원 수
	bool _inviteOnly; // 초대만 가능

	time_t _created;
	time_t _topic_created;
	int _topic_author;

	std::map<int, int> _clients; // fd, operator
	std::set<int> _invited;		 // 초대된 인원
	Channel();

public:
	Channel(std::string const &name);
	~Channel();

	void setName(std::string const &name);
	void setKey(std::string const &key);
	void setLimit(size_t limit);
	void setInviteOnly(bool inviteOnly);

	void setTopic(std::string const &topic, int author_fd);

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
	std::vector<int> getClientsFd();
	int isOperator(int fd) const;
	
	time_t getCreated();
	time_t getTopicCreated();
	int getTopicAuthor();

	int isInvited(int fd);
};

#endif
