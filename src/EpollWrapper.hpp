/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/05/29 12:10:30 by eandre-f         ###   ########.fr       */
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

class EpollWrapper
{
  public:
	EpollWrapper(void);
	~EpollWrapper(void);

  private:
	int epoll_fd;
};

#endif /* EPOLLWRAPPER_HPP */
