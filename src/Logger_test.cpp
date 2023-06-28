/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 13:59:03 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 22:38:47 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include "doctest.h"
#include <iostream>
#include <ostream>
#include <sstream>

std::streambuf *p_cout_streambuf = std::cout.rdbuf();

void cout_redirect(std::ostringstream &oss)
{
	std::cout.rdbuf(oss.rdbuf());
}

void cout_restore()
{
	std::cout.rdbuf(p_cout_streambuf);
}

TEST_SUITE("Logger")
{
	TEST_CASE("none")
	{
		Logger::level(LOGGER_LEVEL_NONE);

		std::ostringstream oss;

		cout_redirect(oss);
		Logger::debug("level");
		Logger::info("level");
		Logger::warning("level");
		Logger::error("level");
		cout_restore();

		CHECK_EQ(oss.str(), "");
	}
	TEST_CASE("error")
	{
		Logger::level(LOGGER_LEVEL_ERROR);

		std::ostringstream oss;

		cout_redirect(oss);
		Logger::debug("level");
		Logger::info("level");
		Logger::warning("level");
		Logger::error("error");
		cout_restore();

		CHECK_EQ(oss.str(), "error\n");
	}
	TEST_CASE("warning")
	{
		Logger::level(LOGGER_LEVEL_WARNING);

		std::ostringstream oss;

		cout_redirect(oss);
		Logger::debug("level");
		Logger::info("level");
		Logger::warning("warning");
		Logger::error("error");
		cout_restore();

		CHECK_EQ(oss.str(), "warning\nerror\n");
	}
	TEST_CASE("info")
	{
		Logger::level(LOGGER_LEVEL_INFO);

		std::ostringstream oss;

		cout_redirect(oss);
		Logger::debug("level");
		Logger::info("info");
		Logger::warning("warning");
		Logger::error("error");
		cout_restore();

		CHECK_EQ(oss.str(), "info\nwarning\nerror\n");
	}
	TEST_CASE("debug")
	{
		Logger::level(LOGGER_LEVEL_DEBUG);

		std::ostringstream oss;

		cout_redirect(oss);
		Logger::debug("debug");
		Logger::info("info");
		Logger::warning("warning");
		Logger::error("error");
		cout_restore();

		CHECK_EQ(oss.str(), "debug\ninfo\nwarning\nerror\n");
	}
}