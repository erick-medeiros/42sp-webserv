/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/30 17:06:44 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

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
#include <vector>

typedef std::pair<std::string, std::string> pair_string_t;
typedef std::list<pair_string_t>            labels_t;

typedef struct
{
	std::string              location;
	std::vector<std::string> http_methods;
	std::string              http_redirection;
	std::string              root;
	std::string              directory_listing;
	std::string              response_is_dir;
	std::set<std::string>    cgi_pass;
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

	uint_t const                     &getPort(void) const;
	std::vector<std::string> const   &getServerNames(void) const;
	std::string const                &getErrorPage(int error);
	std::map<int, std::string> const &getErrorPages(void) const;
	size_t const                     &getClientBodySize() const;
	std::vector<location_t> const    &getLocations() const;
	std::vector<location_t>           getLocations(std::string path) const;
	bool                              hasCGI(std::string path) const;
	const std::string                &getMainRoot(void) const;
	const std::string                &getIndex(void) const;
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
	std::string                _index;
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
	int _setUploadPath(std::string &);
};

#endif /* CONFIG_HPP */
