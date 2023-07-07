/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 14:50:36 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/05 15:21:01 by eandre-f         ###   ########.fr       */
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
		Cookie      cookies;
		std::string key1("test1");
		std::string key2("test2");

		t_cookie cookie1;
		cookie1.name = "name1";
		cookie1.value = "123";

		cookies.set(key1, cookie1);
		CHECK_EQ(cookies.get(cookie1.name, key1).value, cookie1.value);

		t_cookie cookie2;
		cookie2.name = "name2";
		cookie2.value = "hello";

		cookies.set(key2, cookie2);
		CHECK_EQ(cookies.get(cookie2.name, key2).value, cookie2.value);
	}
}
