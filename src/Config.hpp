/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/06 17:51:41 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Cookie.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

typedef std::pair<std::string, std::string> pair_string_t;
typedef std::list<pair_string_t>            labels_t;

typedef struct
{
	std::string                          location;
	std::vector<std::string>             allowed_methods;
	std::vector<std::string>             http_methods;
	std::pair<unsigned int, std::string> http_redirection;
	std::string                          root;
	std::string                          directory_listing;
	std::string                          response_is_dir;
	std::set<std::string>                cgi_pass;
	std::set<std::string>                required_cookie;
	std::vector<t_cookie>                set_cookie;
} location_t;

typedef unsigned int uint_t;

#define FAILURE 1

class Config
{
  public:
	Config(void);
	~Config(void);

	bool isValid(void) const;

	int add(std::string label, std::string value);

	const uint_t &getPort(void) const;
	const size_t &getClientBodySize() const;

	std::vector<std::string> const   &getServerNames(void) const;
	std::string const                &getErrorPage(int error);
	std::map<int, std::string> const &getErrorPages(void) const;
	std::vector<location_t> const    &getLocations() const;
	std::vector<location_t>           getLocations(std::string path) const;
	bool                              hasCGI(std::string path) const;
	const std::string                &getMainRoot(void) const;
	const std::set<std::string>      &getIndex(void) const;
	std::vector<std::string> const   &getAllowedMethods(void) const;
	const std::string                &getUploadPath(void) const;

	// Helpers
	bool directoryListingEnabled(std::string path) const;

	static std::string         readFile(const std::string &filename);
	static std::vector<Config> parseConfig(std::string &filedata);

  private:
	uint_t                     _port;
	std::vector<std::string>   _serverNames;
	std::map<int, std::string> _errorPage;
	size_t                     _clientBodySize;
	std::vector<location_t>    _locations;
	std::string                _mainRoot;
	std::set<std::string>      _index;
	std::vector<std::string>   _allowedMethods;
	std::string                _uploadPath;

	int _setPort(std::string &);
	int _setServerName(std::string &);
	int _setErrorPage(std::string &);
	int _setClientBodySize(std::string &);
	int _setLocation(std::string &);
	int _setHttpMethods(std::string &);
	int _setHttpRedirection(std::string &);
	int _setRoot(std::string &);
	int _setDirectoryListing(std::string &);
	int _setResponseIsDir(std::string &);
	int _setCGI(std::string &);
	int _setMainRoot(std::string &);
	int _setIndex(std::string &);
	int _setAllowedMethods(std::string &);
	int _setRequiredCookie(std::string &);
	int _setSetCookie(std::string &);
	int _isValidHttpVerb(std::string const) const;
	int _setUploadPath(std::string &);
};

#endif /* CONFIG_HPP */
