/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 13:59:03 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 22:41:56 by eandre-f         ###   ########.fr       */
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
		Logger logger;
		logger.level(LOGGER_LEVEL_NONE);

		std::ostringstream oss;

		cout_redirect(oss);
		logger.debug("level");
		logger.info("level");
		logger.warning("level");
		logger.error("level");
		cout_restore();

		CHECK_EQ(oss.str(), "");
	}
	TEST_CASE("error")
	{
		Logger logger;
		logger.level(LOGGER_LEVEL_ERROR);

		std::ostringstream oss;

		cout_redirect(oss);
		logger.debug("level");
		logger.info("level");
		logger.warning("level");
		logger.error("error");
		cout_restore();

		CHECK_EQ(oss.str(), "error\n");
	}
	TEST_CASE("warning")
	{
		Logger logger;
		logger.level(LOGGER_LEVEL_WARNING);

		std::ostringstream oss;

		cout_redirect(oss);
		logger.debug("level");
		logger.info("level");
		logger.warning("warning");
		logger.error("error");
		cout_restore();

		CHECK_EQ(oss.str(), "warning\nerror\n");
	}
	TEST_CASE("info")
	{
		Logger logger;
		logger.level(LOGGER_LEVEL_INFO);

		std::ostringstream oss;

		cout_redirect(oss);
		logger.debug("level");
		logger.info("info");
		logger.warning("warning");
		logger.error("error");
		cout_restore();

		CHECK_EQ(oss.str(), "info\nwarning\nerror\n");
	}
	TEST_CASE("debug")
	{
		Logger logger;
		logger.level(LOGGER_LEVEL_DEBUG);

		std::ostringstream oss;

		cout_redirect(oss);
		logger.debug("debug");
		logger.info("info");
		logger.warning("warning");
		logger.error("error");
		cout_restore();

		CHECK_EQ(oss.str(), "debug\ninfo\nwarning\nerror\n");
	}
}