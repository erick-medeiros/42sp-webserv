/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_test.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 08:27:32 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 08:31:08 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doctest.h"
#include "utils.hpp"

TEST_SUITE("to_string")
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