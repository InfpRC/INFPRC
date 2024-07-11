
#include "Kqueue.hpp"

Kqueue::Kqueue() {
	kq = kqueue();
	if (kq == -1) {
		throw std::runtime_error("kqueue() error");
	}
	timeout.tv_sec = TIME;
	timeout.tv_nsec = 0;
}

Kqueue::~Kqueue() {}

void Kqueue::addEvent(int fd, int filter) {
	EV_SET(&change, fd, filter, EV_ADD|EV_ENABLE, 0, 0, NULL);
	change_list.push_back(change);
}

void Kqueue::delEvent(int fd, int filter) {
	EV_SET(&change, fd, filter, EV_DELETE, 0, 0, NULL);
	change_list.push_back(change);
}

int Kqueue::updateEvent() {
	int size = kevent(kq, &change_list[0], change_list.size(), events, MAX_CLIENTS, &timeout);
	if (size == -1) {
		throw std::runtime_error("kevent() error");
	}
	change_list.clear();
	return size;
}

struct kevent Kqueue::getEvent(int i) {
	return events[i];
}
