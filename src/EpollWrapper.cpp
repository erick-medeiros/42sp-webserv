/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/05/29 19:03:30 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollWrapper.hpp"

EpollWrapper::EpollWrapper(void)
{
	epoll_fd = epoll_create(1);
	if (epoll_fd == -1)
	{
		cerr << "EpollWrapper: Failed to open an epoll file descriptor: "
		     << strerror(errno) << endl;
	}
	std::memset(&events, 0, sizeof(struct epoll_event) * MAX_EVENTS);
}

void EpollWrapper::add(int fd)
{
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		cerr << "EpollWrapper: Failed to add an entry to the interest list of "
		        "the epoll: "
		     << strerror(errno) << endl;
	}
}

void EpollWrapper::remove(int fd)
{
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0) == -1)
	{
		std::cerr << "EpollWrapper: Failed to remove file descriptor from "
		             "epoll: "
		          << strerror(errno) << endl;
	}
}

int EpollWrapper::wait(int timeout)
{
	int fds_ready = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
	if (fds_ready == -1)
	{
		std::cerr << "EpollWrapper: Failed to wait for events on epoll: "
		          << strerror(errno) << std::endl;
	}
	return fds_ready;
}

int EpollWrapper::iter(struct epoll_event *event)
{
	static int it = 0;

	while (it < MAX_EVENTS)
	{
		if (events[it].events & EPOLLIN)
		{
			*event = events[it];
			return it;
		}
		it++;
	}

	it = 0;
	return -1;
}

EpollWrapper::~EpollWrapper(void)
{
	close(epoll_fd);
}
