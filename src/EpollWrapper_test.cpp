/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper_test.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/01 12:24:47 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollWrapper.hpp"
#include "doctest.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

void _pipe(int pipefd[])
{
	if (pipe(pipefd) == -1)
		FAIL("Error in pipe()");
}

void _closepipe(int pipefd[])
{
	close(pipefd[PIPE_READ]);
	close(pipefd[PIPE_WRITE]);
}

TEST_SUITE("EpollWrapper")
{
	TEST_CASE("constructor")
	{
		EpollWrapper();
	}
	TEST_CASE("add file descriptor")
	{
		EpollWrapper epoll;
		int          pipefd[2];
		_pipe(pipefd);

		CHECK_EQ(epoll.add(pipefd[PIPE_READ], EPOLLIN), 0);

		_closepipe(pipefd);
	}
	TEST_CASE("remove file descriptor")
	{
		EpollWrapper epoll;
		int          pipefd[2];
		_pipe(pipefd);

		CHECK_EQ(epoll.add(pipefd[PIPE_READ], EPOLLIN), 0);
		CHECK_EQ(epoll.remove(pipefd[PIPE_READ]), 0);
		CHECK_EQ(epoll.remove(pipefd[PIPE_WRITE]), -1);

		_closepipe(pipefd);
	}
}

TEST_SUITE("EpollWrapper::wait")
{
	TEST_CASE("no monitoring")
	{
		EpollWrapper       epoll;
		const int          maxevents = 1;
		struct epoll_event events[maxevents];
		CHECK_EQ(epoll.wait(events, maxevents, 0), 0);
	}

	TEST_CASE("one monitoring")
	{
		EpollWrapper       epoll;
		const int          maxevents = 1;
		struct epoll_event events[maxevents];

		int pipefd[2];
		_pipe(pipefd);

		CHECK_EQ(epoll.add(pipefd[PIPE_READ], EPOLLIN), 0);
		CHECK_EQ(epoll.wait(events, maxevents, 0), 0);

		{ // write in pipe
			char str[5] = "test";
			write(pipefd[PIPE_WRITE], str, strlen(str));
		}

		{ // level triggered
			CHECK_EQ(epoll.wait(events, maxevents, 0), 1);
			CHECK_EQ(epoll.wait(events, maxevents, 0), 1);
			CHECK_EQ(epoll.wait(events, maxevents, 0), 1);
		}

		SUBCASE("read data of event")
		{
			CHECK_EQ(events[0].events & EPOLLIN, EPOLLIN);
			char buff[2];
			bzero(buff, 2);
			read(events[0].data.fd, buff, 1);
			CHECK_EQ(buff[0], 't');
			CHECK_EQ(epoll.wait(events, maxevents, 0), 1);
			read(events[0].data.fd, buff, 1);
			CHECK_EQ(buff[0], 'e');
			CHECK_EQ(epoll.wait(events, maxevents, 0), 1);
			read(events[0].data.fd, buff, 1);
			CHECK_EQ(buff[0], 's');
			CHECK_EQ(epoll.wait(events, maxevents, 0), 1);
			read(events[0].data.fd, buff, 1);
			CHECK_EQ(buff[0], 't');
			CHECK_EQ(epoll.wait(events, maxevents, 0), 0); // end
		}

		_closepipe(pipefd);
	}

	TEST_CASE("two monitoring")
	{
		EpollWrapper       epoll;
		const int          maxevents = 2;
		struct epoll_event events[maxevents];

		int pipefd_one[2];
		int pipefd_two[2];
		_pipe(pipefd_one);
		_pipe(pipefd_two);

		{ // add
			CHECK_EQ(epoll.add(pipefd_one[PIPE_READ], EPOLLIN), 0);
			CHECK_EQ(epoll.add(pipefd_two[PIPE_READ], EPOLLIN), 0);
			CHECK_EQ(epoll.wait(events, maxevents, 0), 0);
		}

		{ // write in pipes
			char str_alpha[7] = "abcdef";
			char str_num[7] = "123456";
			write(pipefd_one[PIPE_WRITE], str_alpha, strlen(str_alpha));
			write(pipefd_two[PIPE_WRITE], str_num, strlen(str_num));
		}

		{ // level triggered
			CHECK_EQ(epoll.wait(events, maxevents, 0), 2);
			CHECK_EQ(epoll.wait(events, maxevents, 0), 2);
			CHECK_EQ(epoll.wait(events, maxevents, 0), 2);
		}

		{ // check events
			CHECK_EQ(events[0].events & EPOLLIN, EPOLLIN);
			CHECK_EQ(events[0].data.fd, pipefd_one[PIPE_READ]);
			CHECK_EQ(events[1].events & EPOLLIN, EPOLLIN);
			CHECK_EQ(events[1].data.fd, pipefd_two[PIPE_READ]);
		}

		{ // read interspersed
			size_t size_buff = 2;
			char   buff[3];
			bzero(buff, 3);
			//
			read(events[0].data.fd, buff, size_buff);
			CHECK_EQ(buff, string("ab"));
			read(events[1].data.fd, buff, size_buff);
			CHECK_EQ(buff, string("12"));
			CHECK_EQ(epoll.wait(events, maxevents, 0), 2);
			//
			read(events[0].data.fd, buff, size_buff);
			CHECK_EQ(buff, string("cd"));
			read(events[1].data.fd, buff, size_buff);
			CHECK_EQ(buff, string("34"));
			CHECK_EQ(epoll.wait(events, maxevents, 0), 2);
			//
			read(events[0].data.fd, buff, size_buff);
			CHECK_EQ(buff, string("ef"));
			CHECK_EQ(epoll.wait(events, maxevents, 0), 1);
			//
			read(events[1].data.fd, buff, size_buff);
			CHECK_EQ(buff, string("56"));
			CHECK_EQ(epoll.wait(events, maxevents, 0), 0);
		}

		_closepipe(pipefd_one);
		_closepipe(pipefd_two);
	}

	TEST_CASE("one monitoring ~ edge triggered")
	{
		EpollWrapper       epoll;
		const int          maxevents = 1;
		struct epoll_event events[maxevents];

		int pipefd[2];
		_pipe(pipefd);

		// -- edge triggered
		CHECK_EQ(epoll.add(pipefd[PIPE_READ], EPOLLIN | EPOLLET), 0);
		CHECK_EQ(epoll.wait(events, maxevents, 0), 0);

		{ // write in pipe
			char str[5] = "test";
			write(pipefd[PIPE_WRITE], str, strlen(str));
		}

		CHECK_EQ(epoll.wait(events, maxevents, 0), 1);
		CHECK_EQ(epoll.wait(events, maxevents, 0), 0); // -- edge triggered

		_closepipe(pipefd);
	}
}
