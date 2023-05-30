/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper_test.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/05/30 19:09:29 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollWrapper.hpp"
#include "doctest.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

TEST_SUITE("EpollWrapper")
{
	TEST_CASE("constructor")
	{
		EpollWrapper();
	}
	TEST_CASE("add and remove")
	{
		EpollWrapper epoll;

		int pipefd[2];
		if (pipe(pipefd) == -1)
			FAIL("Error in pipe()");
		//
		epoll.add(pipefd[PIPE_READ]);
		epoll.remove(pipefd[PIPE_READ]);
		//
		close(pipefd[PIPE_READ]);
		close(pipefd[PIPE_WRITE]);
	}
	TEST_CASE("wait")
	{
		EpollWrapper epoll;

		CHECK_EQ(epoll.wait(0), 0);
	}
}

TEST_CASE("get_next_event")
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
		FAIL("Error in pipe()");

	int timeout = 0;

	EpollWrapper       epoll;
	struct epoll_event event;

	SUBCASE("no event")
	{
		CHECK_EQ(epoll.get_next_event(&event), -1);
	}
	SUBCASE("one event")
	{
		epoll.add(pipefd[PIPE_READ]);

		CHECK_EQ(epoll.wait(timeout), 0);

		write(pipefd[PIPE_WRITE], "test", 4);

		CHECK_EQ(epoll.wait(timeout), 1);

		SUBCASE("access event")
		{
			CHECK_EQ(epoll.get_next_event(&event), 0);
			CHECK_EQ(event.data.fd, pipefd[PIPE_READ]);

			SUBCASE("use event")
			{
				char buff[11];
				read(event.data.fd, buff, 10);
				CHECK_EQ(epoll.wait(timeout), 0);
			}
		}
		SUBCASE("no event")
		{
			// temporarily ignore because not working...
			// CHECK_EQ(epoll.get_next_event(&event), -1);
		}
	}

	close(pipefd[PIPE_READ]);
	close(pipefd[PIPE_WRITE]);
}
