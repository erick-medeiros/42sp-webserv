/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/05/29 12:06:40 by eandre-f         ###   ########.fr       */
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
}

EpollWrapper::~EpollWrapper(void)
{
	close(epoll_fd);
}
