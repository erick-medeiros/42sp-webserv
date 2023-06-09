/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 14:50:36 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/09 15:06:11 by eandre-f         ###   ########.fr       */
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
}
