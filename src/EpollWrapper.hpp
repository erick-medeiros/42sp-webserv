/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/16 11:33:15 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLLWRAPPER_HPP
#define EPOLLWRAPPER_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "log_utils.hpp"
#include <cstddef>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

using namespace std;

typedef struct
{
	enum channel_type_t
	{
		CHANNEL_SOCKET,
		CHANNEL_CONNECTION
	} type;
	void *ptr;
} channel_t;

class EpollWrapper
{
  public:
	struct epoll_event *events;

	EpollWrapper(size_t maxevents);
	~EpollWrapper(void);

	int add(int fd, epoll_data_t data, uint32_t events);
	int modify(int fd, epoll_data_t data, uint32_t new_events);
	int remove(int fd);

	int wait(int timeout);

  private:
	int    _epoll_fd;
	size_t _maxevents;
};

bool setNonBlocking(int fd);

#endif /* EPOLLWRAPPER_HPP */
