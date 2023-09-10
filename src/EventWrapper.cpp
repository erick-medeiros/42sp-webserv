#include "EventWrapper.hpp"
#ifdef __linux__
#include "EpollWrapper.hpp"
#elif __APPLE__
#include "KqueueWrapper.hpp"
#endif
#include <iostream>

class EpollWrapper;
class KqueueWrapper;

EventWrapper *createEventWrapper()
{
#ifdef __linux__
	std::cout << "-- LINUX detected, using Epoll --" << std::endl;
	return new EpollWrapper(MAX_EVENTS);
#elif __APPLE__
	std::cout << "-- APPLE detected, using Kqueue --" << std::endl;
	return new KqueueWrapper(MAX_EVENTS);
#else
	throw std::runtime_error("Unsupported platform");
#endif
}
