/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 14:50:36 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/09 15:30:10 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COOKIE_HPP
#define COOKIE_HPP

#include "Request.hpp"
#include <cstdlib>
#include <ctime>
#include <map>
#include <sstream>
#include <string>

class Cookie
{
  public:
	Cookie(void);
	~Cookie(void);

	std::string generateSession(void) const;

	static std::string getValueCookie(Request const     &request,
	                                  std::string const &key);

  private:
	std::map<std::string, std::string> _db;
};

#endif /* COOKIE_HPP */
