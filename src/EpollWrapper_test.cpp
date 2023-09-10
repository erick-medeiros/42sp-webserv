/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper_test.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 09:37:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/29 00:12:58 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollWrapper.hpp"
#include "Logger.hpp"
#include "doctest.h"
#include <cstdlib>
#include <iostream>
#include <sys/epoll.h>
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
		Logger::level = LOGGER_LEVEL_NONE;
		EpollWrapper(1);
	}
	TEST_CASE("add file descriptor")
	{
		EpollWrapper epoll(1);
		int          pipefd[2];
		_pipe(pipefd);

		CHECK_EQ(epoll.add(pipefd[PIPE_READ]), 0);

		_closepipe(pipefd);
	}
	TEST_CASE("modify file descriptor")
	{
		const int    maxevents = 1;
		EpollWrapper epoll(maxevents);
		int          pipefd[2];
		_pipe(pipefd);

		write(pipefd[PIPE_WRITE], "test", 4);

		CHECK_EQ(epoll.add(pipefd[PIPE_READ]), 0);
		{
			struct epoll_event &event = epoll.events[0];
			CHECK_EQ(epoll.wait(0), 1);
			CHECK_EQ(event.events, EPOLLIN);
			CHECK_EQ(epoll.modify(event.data.fd, event.data, EPOLLOUT), 0);
			CHECK_EQ(epoll.wait(0), 0);
			CHECK_EQ(epoll.modify(event.data.fd, event.data, EPOLLIN), 0);
			CHECK_EQ(epoll.wait(0), 1);
			CHECK_EQ(event.events, EPOLLIN);
		}
		{
			epoll_data_t data = {0};
			data.fd = pipefd[PIPE_READ];
			CHECK_NE(epoll.modify(pipefd[PIPE_WRITE], data, EPOLLOUT), 0);
		}

		_closepipe(pipefd);
	}
	TEST_CASE("remove file descriptor")
	{
		EpollWrapper epoll(1);
		int          pipefd[2];
		_pipe(pipefd);

		CHECK_EQ(epoll.add(pipefd[PIPE_READ]), 0);
		CHECK_EQ(epoll.remove(pipefd[PIPE_READ]), 0);
		CHECK_EQ(epoll.remove(pipefd[PIPE_WRITE]), -1);

		_closepipe(pipefd);
	}
}

TEST_SUITE("EpollWrapper::wait")
{
	TEST_CASE("no monitoring")
	{
		const int    maxevents = 1;
		EpollWrapper epoll(maxevents);
		CHECK_EQ(epoll.wait(0), 0);
	}

	TEST_CASE("one monitoring")
	{
		const int    maxevents = 1;
		EpollWrapper epoll(maxevents);

		int pipefd[2];
		_pipe(pipefd);

		CHECK_EQ(epoll.add(pipefd[PIPE_READ]), 0);
		CHECK_EQ(epoll.wait(0), 0);

		{ // write in pipe
			char str[5] = "test";
			write(pipefd[PIPE_WRITE], str, strlen(str));
		}

		{ // level triggered
			CHECK_EQ(epoll.wait(0), 1);
			CHECK_EQ(epoll.wait(0), 1);
			CHECK_EQ(epoll.wait(0), 1);
		}

		SUBCASE("read data of event")
		{
			CHECK_EQ(epoll.events[0].events & EPOLLIN, EPOLLIN);
			char buff[2];
			bzero(buff, 2);
			read(epoll.events[0].data.fd, buff, 1);
			CHECK_EQ(buff[0], 't');
			CHECK_EQ(epoll.wait(0), 1);
			read(epoll.events[0].data.fd, buff, 1);
			CHECK_EQ(buff[0], 'e');
			CHECK_EQ(epoll.wait(0), 1);
			read(epoll.events[0].data.fd, buff, 1);
			CHECK_EQ(buff[0], 's');
			CHECK_EQ(epoll.wait(0), 1);
			read(epoll.events[0].data.fd, buff, 1);
			CHECK_EQ(buff[0], 't');
			CHECK_EQ(epoll.wait(0), 0); // end
		}

		_closepipe(pipefd);
	}

	TEST_CASE("two monitoring")
	{
		const int    maxevents = 2;
		EpollWrapper epoll(maxevents);

		int pipefd_one[2];
		int pipefd_two[2];
		_pipe(pipefd_one);
		_pipe(pipefd_two);

		{ // add
			CHECK_EQ(epoll.add(pipefd_one[PIPE_READ]), 0);
			CHECK_EQ(epoll.add(pipefd_two[PIPE_READ]), 0);
			CHECK_EQ(epoll.wait(0), 0);
		}

		{ // write in pipes
			char str_alpha[7] = "abcdef";
			char str_num[7] = "123456";
			write(pipefd_one[PIPE_WRITE], str_alpha, strlen(str_alpha));
			write(pipefd_two[PIPE_WRITE], str_num, strlen(str_num));
		}

		{ // level triggered
			CHECK_EQ(epoll.wait(0), 2);
			CHECK_EQ(epoll.wait(0), 2);
			CHECK_EQ(epoll.wait(0), 2);
		}

		{ // check events
			CHECK_EQ(epoll.events[0].events & EPOLLIN, EPOLLIN);
			CHECK_EQ(epoll.events[0].data.fd, pipefd_one[PIPE_READ]);
			CHECK_EQ(epoll.events[1].events & EPOLLIN, EPOLLIN);
			CHECK_EQ(epoll.events[1].data.fd, pipefd_two[PIPE_READ]);
		}

		{ // read interspersed
			size_t size_buff = 2;
			char   buff[3];
			bzero(buff, 3);
			//
			read(epoll.events[0].data.fd, buff, size_buff);
			CHECK_EQ(buff, std::string("ab"));
			read(epoll.events[1].data.fd, buff, size_buff);
			CHECK_EQ(buff, std::string("12"));
			CHECK_EQ(epoll.wait(0), 2);
			//
			read(epoll.events[0].data.fd, buff, size_buff);
			CHECK_EQ(buff, std::string("cd"));
			read(epoll.events[1].data.fd, buff, size_buff);
			CHECK_EQ(buff, std::string("34"));
			CHECK_EQ(epoll.wait(0), 2);
			//
			read(epoll.events[0].data.fd, buff, size_buff);
			CHECK_EQ(buff, std::string("ef"));
			CHECK_EQ(epoll.wait(0), 1);
			//
			read(epoll.events[1].data.fd, buff, size_buff);
			CHECK_EQ(buff, std::string("56"));
			CHECK_EQ(epoll.wait(0), 0);
		}

		_closepipe(pipefd_one);
		_closepipe(pipefd_two);
	}
}
