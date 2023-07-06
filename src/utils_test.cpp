/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_test.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 08:27:32 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/06 14:35:24 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doctest.h"
#include "utils.hpp"

TEST_SUITE("utils::to_string")
{
	TEST_CASE("int")
	{
		std::string str;
		str = "10";
		CHECK_EQ(utils::to_string(10), str);
		str = "20";
		CHECK_EQ(utils::to_string(20), str);
	}
}

TEST_SUITE("utils::start_with")
{
	TEST_CASE("start")
	{
		CHECK(utils::start_with("world", "wo"));
	}
	TEST_CASE("no start")
	{
		CHECK_FALSE(utils::start_with("world", "awo"));
	}
}

TEST_SUITE("utils::end_with")
{
	TEST_CASE("start")
	{
		CHECK(utils::end_with("world", "ld"));
	}
	TEST_CASE("no start")
	{
		CHECK_FALSE(utils::end_with("world", "lda"));
	}
}

TEST_SUITE("utils::replace")
{
	TEST_CASE("int")
	{
		std::string str = "born$strcode";
		CHECK_EQ(utils::replace(str, "$str", 2), 0);
		CHECK_EQ(str, "born2code");
	}
}
