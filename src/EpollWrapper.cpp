/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/05 19:59:11 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollWrapper.hpp"

EpollWrapper::EpollWrapper()
{
	epoll_fd = epoll_create(1);
	if (epoll_fd == -1)
	{
		cerr << "EpollWrapper: Failed to open an epoll file descriptor: "
		     << strerror(errno) << endl;
	}
}

int EpollWrapper::add(int fd, uint32_t events)
{
	Request *request = new Request(fd);

	struct epoll_event event;
	event.events = events;
	event.data.ptr = request;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		cerr << "EpollWrapper: "
		     << "Failed to add an entry to the interest list of the epoll: "
		     << strerror(errno) << endl;
		return -1;
	}
	return 0;
}

int EpollWrapper::modify(Request *request, uint32_t events)
{
	struct epoll_event event;
	int                fd = request->getFd();
	event.events = events;
	event.data.ptr = request;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event) == -1)
	{
		cerr << "EpollWrapper: "
		     << "Failed to modify an fd in the interest list of the epoll: "
		     << strerror(errno) << endl;
		return -1;
	}
	return 0;
}

int EpollWrapper::remove(int fd)
{
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0) == -1)
	{
		cerr << "EpollWrapper: "
		     << "Failed to remove file descriptor from epoll: " << strerror(errno)
		     << endl;
		return -1;
	}
	return 0;
}

int EpollWrapper::wait(struct epoll_event *events, int maxevents, int timeout)
{
	int fds_ready = epoll_wait(epoll_fd, events, maxevents, timeout);
	if (fds_ready == -1)
	{
		cerr << "EpollWrapper: "
		     << "Failed to wait for events on epoll: " << strerror(errno) << endl;
	}
	return fds_ready;
}

EpollWrapper::~EpollWrapper(void)
{
	close(epoll_fd);
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
