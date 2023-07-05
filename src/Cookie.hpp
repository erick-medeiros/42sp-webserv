/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 14:50:36 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/05 12:10:42 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COOKIE_HPP
#define COOKIE_HPP

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

	std::string const &get(std::string &key);
	void               set(std::string const &key, std::string const &value);

	std::string generateSession(void) const;

  private:
	std::map<std::string, std::string> _db;
};

#endif /* COOKIE_HPP */
