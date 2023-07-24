/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/29 09:44:15 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollWrapper.hpp"

EpollWrapper::EpollWrapper(size_t maxevents) : _maxevents(maxevents)
{
	_epoll_fd = epoll_create(1);
	if (_epoll_fd == -1)
	{
		log.error("epoll_create", strerror(errno));
	}
	events = new epoll_event[_maxevents];
	memset(events, 0, sizeof(epoll_event) * _maxevents);
}

int EpollWrapper::add(int fd)
{
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		log.error("epoll_ctl", strerror(errno));
		return -1;
	}
	return 0;
}

int EpollWrapper::remove(int fd)
{
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, 0) == -1)
	{
		log.error("epoll_ctl", strerror(errno));
		return -1;
	}
	return 0;
}

std::vector<Event> EpollWrapper::getEvents(int timeout)
{
	int fds = epoll_wait(_epoll_fd, events, _maxevents, timeout);
	if (fds == -1)
	{
		log.error("epoll_wait", strerror(errno));
		return std::vector<Event>();
	}
	
	std::vector<Event> eventsVector;
	eventsVector.reserve(fds);

	for (int i = 0; i < fds; ++i) {
		Event event;
		event.fd = events[i].data.fd;
		if (events[i].events & EPOLLERR)
			event.type = ERROR_EVENT;
		else if (events[i].events & EPOLLIN)
			event.type = READ_EVENT;
		else if ( events[i].events & (EPOLLRDHUP | EPOLLHUP))
			event.type = CLOSE_EVENT;
		eventsVector.push_back(event);
	}
	return eventsVector;
}

EpollWrapper::~EpollWrapper(void)
{
	delete[] events;
	close(_epoll_fd);
}

bool setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		log.error("fcntl", strerror(errno));
		return false;
	}
	return true;
}
