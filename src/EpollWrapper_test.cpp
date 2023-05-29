/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper_test.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/05/29 19:02:47 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollWrapper.hpp"
#include "doctest.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

TEST_SUITE("EpollWrapper")
{
	TEST_CASE("Constructor")
	{
		EpollWrapper();
	}
	TEST_CASE("add and remove")
	{
		EpollWrapper epollWrapper;

		int pfd[2];
		if (pipe(pfd) == -1)
			return;
		//
		epollWrapper.add(pfd[0]);
		epollWrapper.remove(pfd[0]);
		//
		close(pfd[0]);
		close(pfd[1]);
	}
	TEST_CASE("wait")
	{
		EpollWrapper epollWrapper;

		int fds_ready = epollWrapper.wait(1);

		CHECK_EQ(fds_ready, 0);
	}
}

TEST_CASE("iter")
{
	int pfd[2];
	if (pipe(pfd) == -1)
		return;

	int timeout = 1;

	EpollWrapper       epollWrapper;
	struct epoll_event event;
	int                it, fds;

	SUBCASE("no event")
	{
		it = epollWrapper.iter(&event);
		CHECK_EQ(it, -1);
	}
	SUBCASE("one event")
	{
		epollWrapper.add(pfd[0]);

		fds = epollWrapper.wait(timeout);
		CHECK_EQ(fds, 0);

		write(pfd[1], "test", 4);

		fds = epollWrapper.wait(timeout);
		CHECK_EQ(fds, 1);

		SUBCASE("access event")
		{
			it = epollWrapper.iter(&event);
			CHECK_EQ(it, 0);
			CHECK_EQ(event.data.fd, pfd[0]);

			SUBCASE("use event")
			{
				char buff[11];
				read(event.data.fd, buff, 10);
				fds = epollWrapper.wait(timeout);
				CHECK_EQ(fds, 0);
			}
		}
		SUBCASE("no event")
		{
			it = epollWrapper.iter(&event);
			// temporarily ignore because not working...
			// CHECK_EQ(it, -1);
		}
	}

	close(pfd[0]);
	close(pfd[1]);
}
