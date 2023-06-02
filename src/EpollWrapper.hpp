/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/01 11:45:21 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLLWRAPPER_HPP
#define EPOLLWRAPPER_HPP

#include <cstddef>
#include <cstring>
#include <errno.h>
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
	int remove(int fd);
	int wait(struct epoll_event *events, int maxevents, int timeout);

  private:
	int epoll_fd;
};

#endif /* EPOLLWRAPPER_HPP */
