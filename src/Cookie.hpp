/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 14:50:36 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/05 15:27:26 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COOKIE_HPP
#define COOKIE_HPP

#include <cstdlib>
#include <ctime>
#include <map>
#include <sstream>
#include <string>

typedef struct
{
	std::string name;
	std::string value;
	std::string domain;
	std::string path;
	std::string expires;
	std::string size;
	bool        httpOnly;
	bool        secure;
	std::string samesite;
	std::string partitionKey;
	std::string priority;
	bool        sessionValue;
} t_cookie;

class Cookie
{
  public:
	Cookie(void);
	~Cookie(void);

	const t_cookie &get(const std::string &name, const std::string &key);
	void            set(const std::string &key, t_cookie &cookie);

	std::string generateSession(void) const;

  private:
	std::map<std::string, std::map<std::string, t_cookie> /**/> _db;
};

#endif /* COOKIE_HPP */
