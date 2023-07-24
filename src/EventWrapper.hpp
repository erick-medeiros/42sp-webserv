#ifndef EVENT_WRAPPER_H
#define EVENT_WRAPPER_H

#include <vector>

enum EventType
{
	ERROR_EVENT,
	READ_EVENT,
	CLOSE_EVENT
};

struct Event
{
	int       fd;
	EventType type;
};

class EventWrapper
{
  public:
	virtual ~EventWrapper() {}
	virtual int                add(int fd) = 0;
	virtual int                remove(int fd) = 0;
	virtual std::vector<Event> getEvents(int timeout) = 0;
};

#endif // EVENT_WRAPPER_H
