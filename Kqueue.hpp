
#ifndef K_QUEUE_HPP
#define K_QUEUE_HPP

#include <iostream>
#include <vector>

#include <sys/event.h>
#include <sys/time.h>

#define MAX_CLIENTS 1024
#define TIME 5

class Kqueue {
	private:
		int kq;
		std::vector<struct kevent> change_list;
		struct kevent change;
		struct kevent events[MAX_CLIENTS];
		struct timespec timeout;
	
	public:
		Kqueue();
		~Kqueue();

		void addEvent(int fd, int filter);
		void delEvent(int fd, int filter);
		int updateEvent();
		struct kevent getEvent(int i);
};

#endif
