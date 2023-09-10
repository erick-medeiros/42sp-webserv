#ifndef KQUEUE_WRAPPER_H
#define KQUEUE_WRAPPER_H

#include "EventWrapper.hpp"

class KqueueWrapper : public EventWrapper
{
  private:
	int    _kqueue_fd;
	size_t _maxevents;

  public:
	struct kevent *events;

	KqueueWrapper(size_t maxevents);
	~KqueueWrapper();

	int                add(int fd);
	int                remove(int fd);
	std::vector<Event> getEvents(int timeout);
};

#endif // KQUEUE_WRAPPER_H
