
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <set>
#include <vector>
#include <sstream>
#include "Client.hpp"

#define CHAN_MEM 0
#define CHAN_OPR 1

class Channel
{
private:
	std::string _name;
	std::string _topic;
	std::string _key; // 비밀번호
	int _limit;	  // 최대 인원 수
	bool _invite_only; // 초대만 가능
	bool _topic_only; // 토픽 권한

	time_t _created;
	time_t _topic_created;
	std::string _topic_author;

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
	void setTopicOnly(bool inviteOnly);
	void setTopic(std::string const &topic, std::string author);

	void inviteClient(int fd);
	void delInvitedClient(int fd);
	void addClient(int fd, int chanops);
	void delClient(int fd);
	void addOperator(int fd);
	void delOperator(int fd);

	size_t getClientNum();
	std::string const &getName() const;
	std::string const &getTopic() const;
	std::string const &getKey() const;
	int getLimit() const;
	bool getInviteOnly() const;
	bool getTopicOnly() const;
	std::map<int, int> &getClients();
	std::vector<int> getClientsFd();
	std::string getModeList();


	std::string getCreated();
	std::string getTopicCreated();
	std::string getTopicAuthor();

	int isInvited(int fd);
	

};

#endif
