/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 14:50:36 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/09 15:31:11 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

Cookie::Cookie(void)
{
	srand(time(0));
}

Cookie::~Cookie(void) {}

std::string Cookie::generateSession(void) const
{
	std::string        session;
	std::ostringstream oss;
	oss << rand();
	session = oss.str();
	if (_db.count(session) > 0)
		return generateSession();
	return session;
};

// static

std::string Cookie::getValueCookie(Request const &request, std::string const &key)
{
	std::string headerCookie = request.getHeaderValue("Cookie");
	std::string value;

	{
		std::istringstream sstream(headerCookie);
		std::string        word;
		std::getline(sstream, word, '=');
		if (word == key)
		{
			std::getline(sstream, word, '=');
			return word;
		}
		return "";
	}

	return value;
}
