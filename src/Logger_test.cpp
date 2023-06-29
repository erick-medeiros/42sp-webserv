/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 13:59:03 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/29 09:41:33 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include "doctest.h"
#include <iostream>
#include <ostream>
#include <sstream>

TEST_SUITE("Logger")
{
	TEST_CASE("none")
	{
		std::ostringstream oss;

		Logger logger(oss);
		Logger::level = LOGGER_LEVEL_NONE;

		logger.debug("debug");
		logger.info("info");
		logger.warning("warning");
		logger.error("error");

		CHECK_EQ(oss.str(), "");
	}
	TEST_CASE("error")
	{
		std::ostringstream oss;

		Logger logger(oss);
		Logger::level = LOGGER_LEVEL_ERROR;

		logger.debug("debug");
		logger.info("info");
		logger.warning("warning");

		CHECK_EQ(oss.str(), "");

		SUBCASE("show")
		{
			std::ostringstream oss;
			Logger             logger(oss);
			logger.error("error");
			CHECK_NE(oss.str().find("error"), std::string::npos);
		}
	}
	TEST_CASE("warning")
	{
		std::ostringstream oss;

		Logger logger(oss);
		Logger::level = LOGGER_LEVEL_WARNING;

		logger.debug("debug");
		logger.info("info");

		CHECK_EQ(oss.str(), "");

		SUBCASE("show")
		{
			std::ostringstream oss;
			Logger             logger(oss);
			logger.warning("warning");
			logger.error("error");
			CHECK_NE(oss.str().find("warning"), std::string::npos);
			CHECK_NE(oss.str().find("error"), std::string::npos);
		}
	}
	TEST_CASE("info")
	{
		std::ostringstream oss;

		Logger logger(oss);
		Logger::level = LOGGER_LEVEL_INFO;

		logger.debug("debug");

		CHECK_EQ(oss.str(), "");

		SUBCASE("")
		{
			std::ostringstream oss;
			Logger             logger(oss);

			logger.info("info");
			logger.warning("warning");
			logger.error("error");

			CHECK_NE(oss.str().find("info"), std::string::npos);
			CHECK_NE(oss.str().find("warning"), std::string::npos);
			CHECK_NE(oss.str().find("error"), std::string::npos);
		}
	}
	TEST_CASE("debug")
	{
		std::ostringstream oss;

		Logger logger(oss);
		Logger::level = LOGGER_LEVEL_DEBUG;

		logger.info("debug");
		logger.info("info");
		logger.warning("warning");
		logger.error("error");

		CHECK_NE(oss.str().find("debug"), std::string::npos);
		CHECK_NE(oss.str().find("info"), std::string::npos);
		CHECK_NE(oss.str().find("warning"), std::string::npos);
		CHECK_NE(oss.str().find("error"), std::string::npos);
	}
}

TEST_SUITE("Logger extra error")
{
	TEST_CASE("int")
	{
		std::ostringstream oss;
		Logger             log(oss);
		Logger::showColor = false;

		log.error("error", 1);

		CHECK_EQ(oss.str(), "error: 1\n");
	}
	TEST_CASE("char *")
	{
		std::ostringstream oss;
		Logger             log(oss);
		Logger::showColor = false;

		char msg[] = "msg";
		log.error("error", msg);

		CHECK_EQ(oss.str(), "error: msg\n");
	}
}
