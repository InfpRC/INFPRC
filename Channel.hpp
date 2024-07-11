
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>

#include "Client.hpp"

class Channel : public std::map< int, Client > {
	private:

	public:
		Channel();
		~Channel();

		void initClient(int fd);
		void delClient(int fd);
		Client &getClient(int fd);
};

#endif
