/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 23:35:22 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLLWRAPPER_HPP
#define EPOLLWRAPPER_HPP

#include "Config.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "EventWrapper.hpp"
#include <cstddef>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

class EpollWrapper  : public EventWrapper 
{
  public:
	struct epoll_event *events;

	EpollWrapper(size_t maxevents);
	~EpollWrapper();

	int add(int fd);
	int modify(int fd, epoll_data_t data, uint32_t new_events);
	int remove(int fd);
	int wait(int timeout);

	std::vector<Event> getEvents(int timeout);

  private:
	int    _epoll_fd;
	size_t _maxevents;
};

bool setNonBlocking(int fd);

#endif /* EPOLLWRAPPER_HPP */
