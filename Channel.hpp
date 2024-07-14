
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>

#include "Client.hpp"


class Channel {
	private:
		std::string _name;
		std::string _topic;
		std::string _mode;
		std::map<int, int> _clients; // fd, operator
		Channel();


	public:
		Channel(std::string const &name, std::string const &topic, std::string const &mode);
		~Channel();

		void setName(std::string const &name);
		void setTopic(std::string const &topic);
		void setMode(std::string const &mode);
		void addClient(int fd, int chanops);
		void delClient(int fd);

		std::string const &getName() const;
		std::string const &getTopic() const;
		std::string const &getMode() const;
};

#endif
