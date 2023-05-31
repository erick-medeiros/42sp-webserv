/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/05/30 18:44:35 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLLWRAPPER_HPP
#define EPOLLWRAPPER_HPP

#include <cstring>
#include <errno.h>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

using namespace std;

#define MAX_EVENTS 10

class EpollWrapper
{
  public:
	EpollWrapper(void);
	~EpollWrapper(void);

	void add(int fd);
	void remove(int fd);
	int  wait(int timeout);

	int getNextEvent(struct epoll_event *event);

  private:
	int                epoll_fd;
	struct epoll_event events[MAX_EVENTS];
};

#endif /* EPOLLWRAPPER_HPP */
