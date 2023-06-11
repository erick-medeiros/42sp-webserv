/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/11 11:23:11 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "log_utils.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

typedef pair<string, string> pair_string_t;
typedef list<pair_string_t>  labels_t;

typedef struct
{
	string         location;
	vector<string> http_methods;
	string         http_redirection;
	string         root;
	string         directory_listing;
	string         response_is_dir;
} location_t;

typedef struct
{
	string path;
	string extension;
} cgi_t;

#define FAILURE 1

class Config
{
  public:
	Config(void);
	~Config(void);

	bool isValid(void) const;

	int add(string label, string value);

	vector<int> const        &getPorts(void) const;
	vector<string> const     &getServerNames(void) const;
	string const             &getErrorPage(int error);
	size_t const             &getClientBodySize() const;
	vector<location_t> const &getLocations() const;
	cgi_t const              &getCGI(void) const;

	static string         readFile(const string &filename);
	static vector<Config> parseConfig(string &filedata);

  private:
	vector<int>        _ports;
	vector<string>     _serverNames;
	map<int, string>   _errorPage;
	size_t             _clientBodySize;
	vector<location_t> _locations;
	cgi_t              _cgi;

	int _setPorts(string &);
	int _setServerName(string &);
	int _setErrorPage(string &);
	int _setClientBodySize(string &);
	int _setLocation(string &);
	int _setHttpMethods(string &);
	int _setHttpRedirection(string &);
	int _setRoot(string &);
	int _setDirectoryListing(string &);
	int _setResponseIsDir(string &);
	int _setCGI(string &);
};

#endif /* CONFIG_HPP */
