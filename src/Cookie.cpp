/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 14:50:36 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/09 15:57:44 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

Cookie::Cookie(void)
{
	srand(time(0));
}

Cookie::~Cookie(void) {}

std::string const &Cookie::get(std::string &key)
{
	return _db[key];
}

void Cookie::set(std::string const &key, std::string const &value)
{
	_db[key] = value;
}

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
	std::string headerCookie;
	std::string value;

	try
	{
		headerCookie = request.getHeaderValue("Cookie");
	}
	catch (...)
	{
		return "";
	};

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

std::string Cookie::getUsername(Request const &request)
{
	std::string        text(request.getResourceQuery());
	std::istringstream sstream(text);
	std::string        word;

	while (std::getline(sstream, word, '&'))
	{
		std::istringstream sstream(word);
		std::string        key;

		std::getline(sstream, key, '=');
		if (key == "username")
		{
			std::getline(sstream, key, '=');
			return key;
		}
	}

	return "";
}
