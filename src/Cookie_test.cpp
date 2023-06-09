/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 14:50:36 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/09 17:49:54 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"
#include "doctest.h"

TEST_SUITE("Class Cookie")
{
	TEST_CASE("Constructor")
	{
		Cookie cookies;
	}
	TEST_CASE("generateSession")
	{
		Cookie cookie;
		CHECK_NE(cookie.generateSession(), cookie.generateSession());
	}
	TEST_CASE("set and get")
	{
		Cookie      cookie;
		std::string key("test");
		std::string key2("test2");

		cookie.set(key, "123");
		CHECK_EQ(cookie.get(key), "123");
		cookie.set(key2, "hello");
		CHECK_EQ(cookie.get(key2), "hello");
		cookie.set(key, "1234");
		CHECK_EQ(cookie.get(key), "1234");
	}
}
