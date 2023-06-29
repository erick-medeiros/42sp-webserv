/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 13:59:03 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/29 00:14:03 by eandre-f         ###   ########.fr       */
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
		Logger::level = LOGGER_LEVEL_NONE;

		std::ostringstream oss;

		cout_redirect(oss);
		logger.debug("debug");
		logger.info("info");
		logger.warning("warning");
		logger.error("error");
		cout_restore();

		CHECK_EQ(oss.str(), "");
	}
	TEST_CASE("error")
	{
		Logger logger;
		Logger::level = LOGGER_LEVEL_ERROR;

		std::ostringstream oss;

		cout_redirect(oss);
		logger.debug("debug");
		logger.info("info");
		logger.warning("warning");
		cout_restore();
		CHECK_EQ(oss.str(), "");

		SUBCASE("")
		{
			std::ostringstream oss;
			cout_redirect(oss);
			logger.error("error");
			cout_restore();
			CHECK_NE(oss.str().find("error"), std::string::npos);
		}
	}
	TEST_CASE("warning")
	{
		Logger logger;
		Logger::level = LOGGER_LEVEL_WARNING;

		std::ostringstream oss;

		cout_redirect(oss);
		logger.debug("debug");
		logger.info("info");
		cout_restore();
		CHECK_EQ(oss.str(), "");

		SUBCASE("")
		{
			std::ostringstream oss;
			cout_redirect(oss);
			logger.warning("warning");
			logger.error("error");
			cout_restore();
			CHECK_NE(oss.str().find("warning"), std::string::npos);
			CHECK_NE(oss.str().find("error"), std::string::npos);
		}
	}
	TEST_CASE("info")
	{
		Logger logger;
		Logger::level = LOGGER_LEVEL_INFO;

		std::ostringstream oss;

		cout_redirect(oss);
		logger.debug("debug");
		cout_restore();
		CHECK_EQ(oss.str(), "");

		SUBCASE("")
		{
			std::ostringstream oss;
			cout_redirect(oss);
			logger.info("info");
			logger.warning("warning");
			logger.error("error");
			cout_restore();
			CHECK_NE(oss.str().find("info"), std::string::npos);
			CHECK_NE(oss.str().find("warning"), std::string::npos);
			CHECK_NE(oss.str().find("error"), std::string::npos);
		}
	}
	TEST_CASE("debug")
	{
		Logger logger;
		Logger::level = LOGGER_LEVEL_DEBUG;

		std::ostringstream oss;
		cout_redirect(oss);
		logger.info("debug");
		logger.info("info");
		logger.warning("warning");
		logger.error("error");
		cout_restore();

		CHECK_NE(oss.str().find("debug"), std::string::npos);
		CHECK_NE(oss.str().find("info"), std::string::npos);
		CHECK_NE(oss.str().find("warning"), std::string::npos);
		CHECK_NE(oss.str().find("error"), std::string::npos);
	}
}