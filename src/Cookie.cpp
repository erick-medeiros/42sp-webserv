/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 14:50:36 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/05 12:14:05 by eandre-f         ###   ########.fr       */
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
