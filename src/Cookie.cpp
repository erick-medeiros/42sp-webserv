/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 14:50:36 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/05 13:31:14 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

Cookie::Cookie(void)
{
	srand(time(0));
}

Cookie::~Cookie(void) {}

const t_cookie &Cookie::get(const std::string &name, const std::string &key)
{
	return _db[name][key];
}

void Cookie::set(const std::string &key, t_cookie &cookie)
{
	_db[cookie.name][key] = cookie;
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
