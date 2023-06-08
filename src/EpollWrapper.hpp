/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/05 19:57:46 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLLWRAPPER_HPP
#define EPOLLWRAPPER_HPP

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

class EpollWrapper
{
  public:
	EpollWrapper(void);
	~EpollWrapper(void);

	int add(int fd, uint32_t events);
	int modify(epoll_event event, uint32_t events);
	int remove(int fd);
	int wait(struct epoll_event *events, int maxevents, int timeout);

  private:
	int epoll_fd;
};

bool setNonBlocking(int fd);

#endif /* EPOLLWRAPPER_HPP */
