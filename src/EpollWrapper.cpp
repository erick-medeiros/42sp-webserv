/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/01 11:47:51 by eandre-f         ###   ########.fr       */
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
	struct epoll_event event;
	event.events = events;
	event.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		cerr << "EpollWrapper: "
		     << "Failed to add an entry to the interest list of the epoll: "
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
