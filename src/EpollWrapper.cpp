/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/08 12:10:34 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollWrapper.hpp"

EpollWrapper::EpollWrapper(size_t maxevents) : _maxevents(maxevents)
{
	_epoll_fd = epoll_create(1);
	if (_epoll_fd == -1)
	{
		logError("EpollWrapper: epoll_create: ", strerror(errno));
	}
	events = new epoll_event[_maxevents];
}

int EpollWrapper::add(int fd, epoll_data_t data, uint32_t events)
{
	struct epoll_event event;
	event.events = events;
	event.data = data;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		logError("EpollWrapper: add: ", strerror(errno));
		return -1;
	}
	return 0;
}

int EpollWrapper::modify(int fd, epoll_data_t data, uint32_t new_events)
{
	struct epoll_event event;
	event.events = new_events;
	event.data = data;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &event) == -1)
	{
		logError("EpollWrapper: modify: ", strerror(errno));
		return -1;
	}
	return 0;
}

int EpollWrapper::remove(int fd)
{
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, 0) == -1)
	{
		logError("EpollWrapper: remove: ", strerror(errno));
		return -1;
	}
	return 0;
}

int EpollWrapper::wait(int timeout)
{
	int fds = epoll_wait(_epoll_fd, events, _maxevents, timeout);
	if (fds == -1)
	{
		logError("EpollWrapper: epoll_wait: ", strerror(errno));
	}
	return fds;
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
		logError("--- Error: set nonblocking for fd", fd);
		return false;
	}
	return true;
}
