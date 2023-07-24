#include "EventWrapper.hpp"
#ifdef __linux__
#include "EpollWrapper.hpp"
#elif __APPLE__
#include "KqueueWrapper.hpp"
#endif


class EpollWrapper;
class KqueueWrapper;

EventWrapper* createEventWrapper() {
#ifdef __linux__
    return new EpollWrapper(MAX_EVENTS);
#elif __APPLE__
    return new KqueueWrapper(MAX_EVENTS);
#else
    throw std::runtime_error("Unsupported platform");
#endif
}
