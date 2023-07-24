#include "KqueueWrapper.hpp"
#include <iostream>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

KqueueWrapper::KqueueWrapper(size_t maxevents) : _maxevents(maxevents)
{
	_kqueue_fd = kqueue();
	if (_kqueue_fd == -1)
	{
		std::cerr << "Failed to create kqueue" << std::endl;
	}
	events = new struct kevent[_maxevents];
}

KqueueWrapper::~KqueueWrapper()
{
	close(_kqueue_fd);
}

int KqueueWrapper::add(int fd)
{
	struct kevent ev;
	EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(_kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
	{
		return -1;
	}

	EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	return kevent(_kqueue_fd, &ev, 1, NULL, 0, NULL);
}

int KqueueWrapper::remove(int fd)
{
	struct kevent ev;
	EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(_kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
	{
		return -1;
	}

	EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	return kevent(_kqueue_fd, &ev, 1, NULL, 0, NULL);
}

std::vector<Event> KqueueWrapper::getEvents(int timeout)
{
	struct timespec ts;
	ts.tv_sec = timeout / 1000;
	ts.tv_nsec = (timeout % 1000) * 1000000;

	int num_events = kevent(_kqueue_fd, NULL, 0, events, 10, &ts);
	if (num_events == -1)
	{
		// Handle error
		return std::vector<Event>();
	}

	std::vector<Event> result;
	result.reserve(num_events);

	for (int i = 0; i < num_events; ++i)
	{
		int       fd = events[i].ident;
		EventType type = ERROR_EVENT;

		if (events[i].filter == EVFILT_READ || events[i].filter == EVFILT_WRITE)
			type = READ_EVENT;
		else if (events[i].flags & EV_EOF)
			type = CLOSE_EVENT;

		Event event;
		event.fd = fd;
		event.type = type;
		result.push_back(event);
	}

	return result;
}
