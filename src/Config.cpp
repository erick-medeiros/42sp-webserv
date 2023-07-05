/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/04 21:09:37 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(void)
    : _port(0), _clientBodySize(1024 * 1024), _mainRoot("."), _index("index.html")
{
	this->_allowedMethods.push_back("GET");
	this->_allowedMethods.push_back("POST");
	this->_allowedMethods.push_back("DELETE");
}

Config::~Config(void) {}

int Config::add(std::string label, std::string value)
{
	log.debug("config: " + label + ": " + value);
	if (label == "port")
		return _setPort(value);
	if (label == "server_name")
		return _setServerName(value);
	if (label == "error_page")
		return _setErrorPage(value);
	if (label == "client_max_body_size")
		return _setClientBodySize(value);
	if (label == "root")
		return _setMainRoot(value);
	if (label == "index")
		return _setIndex(value);
	if (label == "allowed_methods")
		return _setAllowedMethods(value);
	if (label == "location_http_methods")
		return _setHttpMethods(value);
	if (label == "location")
		return _setLocation(value);
	if (label == "location_http_redirection")
		return _setHttpRedirection(value);
	if (label == "location_root")
		return _setRoot(value);
	if (label == "location_directory_listing")
		return _setDirectoryListing(value);
	if (label == "location_response_is_dir")
		return _setResponseIsDir(value);
	if (label == "location_cgi_pass")
		return _setCGI(value);
	if (label == "location_required_cookie")
		return _setRequiredCookie(value);
	log.error("config label not match: " + label);
	return 1;
}

bool Config::isValid(void) const
{
	if (_port == 0)
		return false;
	return true;
}

int Config::_setPort(std::string &value)
{
	std::stringstream ss(value);
	int               port;

	if (_port != 0)
	{
		log.error("port: exist");
		return FAILURE;
	}

	ss >> port;

	if (port < 1024 || port > 49151)
	{
		log.error("port: range invalid");
		return FAILURE;
	}

	if (!ss.eof())
	{
		log.error("port: error value");
		return FAILURE;
	}

	_port = port;

	return 0;
}

int Config::_setServerName(std::string &value)
{
	if (_serverNames.size() > 0)
	{
		log.error("server_name: exist");
		return FAILURE;
	}
	if (value == "")
	{
		log.error("server_name empty");
		return FAILURE;
	}

	std::stringstream ss(value);
	std::string       server_name;

	while (!ss.eof())
	{
		ss >> server_name;
		// TODO: Checar se a string contem caracteres validos para um server_name
		_serverNames.push_back(server_name);
	}

	return 0;
}

int Config::_setErrorPage(std::string &value)
{
	std::stringstream ss(value);
	int               error = 0;
	std::string       page;
	ss >> error;
	ss >> page;
	if (!ss.eof())
		return FAILURE;

	if (error < 400 || error > 599)
	{
		log.error("error_page: range err: " + value);
		return FAILURE;
	}
	_errorPage[error] = page;
	return 0;
}

int Config::_setClientBodySize(std::string &value)
{
	std::stringstream ss(value);
	ss >> _clientBodySize;
	if (_clientBodySize == 0)
		return FAILURE;

	if (value.size() == 0)
		return FAILURE;

	char last = value[value.size() - 1];
	if (last == 'M')
		_clientBodySize *= 1024 * 1024;
	else if (last == 'K')
		_clientBodySize *= 1024;
	else
		return FAILURE;

	return 0;
}

int Config::_setLocation(std::string &value)
{
	location_t location;
	location.location = value;
	location.http_methods.push_back("GET");
	location.http_methods.push_back("POST");
	location.http_methods.push_back("DELETE");
	_locations.push_back(location);
	return 0;
}

int Config::_setHttpMethods(std::string &value)
{
	if (value.empty())
	{
		return 1;
	}

	std::vector<location_t>::reference location = _locations.back();
	std::stringstream                  ss(value);

	std::vector<std::string> temp;
	while (!ss.eof())
	{
		std::string verb;
		ss >> verb;
		verb = utils::trim(verb);
		if (!_isValidHttpVerb(verb) || !utils::contains(_allowedMethods, verb))
		{
			location.http_methods.clear();
			return 1;
		}

		if (utils::contains(temp, verb))
		{
			continue;
		}

		temp.push_back(verb);
	}

	std::vector<std::string> methods;
	for (size_t i = 0; i < temp.size(); ++i)
	{
		for (size_t j = 0; j < _allowedMethods.size(); ++j)
		{
			if (temp[i] == _allowedMethods[j])
			{
				methods.push_back(temp[i]);
			}
		}
	}

	location.http_methods.clear();
	location.http_methods = methods;
	return 0;
}

int Config::_setHttpRedirection(std::string &value)
{
	if (_locations.size() == 0)
		return 1;
	std::stringstream                  ss(value);
	std::vector<location_t>::reference location = _locations.back();

	location.http_redirection = value;

	return 0;
}

int Config::_setRoot(std::string &value)
{
	if (_locations.size() == 0)
		return 1;
	std::stringstream                  ss(value);
	std::vector<location_t>::reference location = _locations.back();

	location.root = value;

	return 0;
}

int Config::_setDirectoryListing(std::string &value)
{
	if (_locations.size() == 0)
		return 1;
	std::stringstream                  ss(value);
	std::vector<location_t>::reference location = _locations.back();

	location.directory_listing = value;

	return 0;
}

bool Config::directoryListingEnabled(std::string path) const
{
	std::vector<location_t>::const_iterator it;
	for (it = _locations.begin(); it != _locations.end(); ++it)
	{
		if (it->location == path)
			return it->directory_listing == "on";
	}
	return false;
}

int Config::_setResponseIsDir(std::string &value)
{
	if (_locations.size() == 0)
		return 1;
	std::stringstream                  ss(value);
	std::vector<location_t>::reference location = _locations.back();

	location.response_is_dir = value;

	return 0;
}

int Config::_setCGI(std::string &value)
{
	std::vector<location_t>::reference location = _locations.back();

	std::istringstream iss(value);

	std::string token;
	while (iss >> token)
	{
		location.cgi_pass.insert(token);
	}

	return 0;
}

int Config::_setAllowedMethods(std::string &value)
{
	std::vector<std::string> methods;
	std::stringstream        ss(value);

	while (!ss.eof())
	{
		std::string httpVerb;
		ss >> httpVerb;
		httpVerb = utils::trim(httpVerb);

		if (httpVerb == "GET" || httpVerb == "POST" || httpVerb == "DELETE")
		{
			if (utils::contains(methods, httpVerb))
			{
				continue;
			}
			methods.push_back(httpVerb);
		}
		else
		{
			return 1;
		}
	}

	this->_allowedMethods.clear();
	this->_allowedMethods = methods;
	return 0;
}

std::vector<std::string> const &Config::getAllowedMethods(void) const
{
	return _allowedMethods;
}

int Config::_setMainRoot(std::string &value)
{
	_mainRoot = value;
	return 0;
}

int Config::_setIndex(std::string &value)
{
	_index = value;
	return 0;
}

int Config::_setRequiredCookie(std::string &value)
{
	std::vector<location_t>::reference location = _locations.back();

	std::stringstream ss(value);

	while (!ss.eof())
	{
		std::string value;
		ss >> value;
		value = utils::trim(value);
		location.required_cookie.insert(value);
	}

	return 0;
}

uint_t const &Config::getPort(void) const
{
	return _port;
}

std::vector<std::string> const &Config::getServerNames(void) const
{
	return _serverNames;
}

size_t const &Config::getClientBodySize() const
{
	return _clientBodySize;
}

std::string const &Config::getErrorPage(int error)
{
	return _errorPage[error];
}

std::map<int, std::string> const &Config::getErrorPages() const
{
	return _errorPage;
}

std::vector<location_t> const &Config::getLocations() const
{
	return _locations;
}

const std::string &Config::getMainRoot(void) const
{
	return _mainRoot;
}

const std::string &Config::getIndex(void) const
{
	return _index;
}

// Return the locations that match the path
std::vector<location_t> Config::getLocations(std::string path) const
{
	std::vector<location_t>                 locations;
	std::vector<location_t>::const_iterator it;
	for (it = _locations.begin(); it != _locations.end(); ++it)
	{
		if (path.find(it->location) == 0)
			locations.push_back(*it);
	}
	// TODO (opcional) ordenar por "nesting" (mais especifico primeiro)
	return locations;
}

bool Config::hasCGI(std::string path) const
{
	std::string fileExtenstion = "." + path.substr(path.find_last_of(".") + 1);

	log.warning("!!! fileExtenstion: " + fileExtenstion);

	// If there is no file extension, it is not a CGI
	if (fileExtenstion.empty())
		return false;

	std::vector<location_t>                 locations = getLocations(path);
	std::vector<location_t>::const_iterator it;
	for (it = locations.begin(); it != locations.end(); ++it)
	{
		log.warning("!!! it->location: " + it->location);

		if (it->cgi_pass.find(fileExtenstion) != it->cgi_pass.end())
			return true;
	}
	return false;
}

std::string Config::readFile(const std::string &filename)
{
	std::ifstream     file(filename.c_str());
	std::stringstream buffer;

	if (file)
	{
		buffer << file.rdbuf();
		file.close();
	}
	else
	{
		log.error("Error: readFile: " + filename);
		return "";
	}

	return buffer.str();
}

static void _panic(std::string err)
{
	throw std::runtime_error("Panic: " + err);
}

static std::list<std::string> _getLineFromFileData(std::string &filedata)
{
	std::list<std::string> lines;
	std::stringstream      ss(filedata);

	std::string line;
	while (getline(ss, line))
	{
		line = utils::trim(line);
		if (line.size() > 0)
		{
			if (line.size() > 1 && utils::end_with(line, "{"))
			{
				line.resize(line.size() - 1);
				line = utils::trim(line);
				lines.push_back(line);
				lines.push_back("{");
				continue;
			}
			lines.push_back(line);
		}
	}
	return lines;
}

static std::pair<std::string, std::string> getLabel(std::string &line)
{
	std::pair<std::string, std::string> content;
	std::istringstream                  ss(line);
	ss >> content.first;
	content.second = line.substr(content.first.size());
	content.second = utils::trim(content.second);
	return content;
}

static std::string extractFront(std::list<std::string> &lines)
{
	std::string str = lines.front();
	lines.pop_front();
	return str;
}

int Config::_isValidHttpVerb(std::string const str) const
{
	if (str == "GET" || str == "POST" || str == "DELETE")
	{
		return 1;
	}
	return 0;
}

static std::list<labels_t> _getLabels(std::list<std::string> &lines)
{
	std::list<labels_t> configs;

	bool populateServer = false;

	labels_t config;

	while (!lines.empty())
	{
		if (utils::start_with(lines.front(), "#"))
		{
			extractFront(lines);
			continue;
		}

		if (!populateServer)
		{
			if (extractFront(lines) != "server")
				_panic("1");
			if (extractFront(lines) != "{")
				_panic("2");
			populateServer = true;
			continue;
		}

		std::string line = extractFront(lines);

		if (populateServer && utils::start_with(line, "location"))
		{
			config.push_back(getLabel(line));
			if (extractFront(lines) != "{")
				_panic("3");
			while (!lines.empty())
			{
				std::string line = extractFront(lines);
				if (line == "}")
					break;
				pair_string_t data = getLabel(line);
				data.first = "location_" + data.first;
				config.push_back(data);
			}
			continue;
		}

		if (populateServer && line == "}")
		{
			populateServer = false;
			configs.push_back(config);
			config.clear();
			continue;
		}

		config.push_back(getLabel(line));
	}
	if (populateServer)
		_panic("4");
	return configs;
}

std::vector<Config> Config::parseConfig(std::string &filedata)
{
	std::list<std::string> lines = _getLineFromFileData(filedata);
	std::list<labels_t>    configs = _getLabels(lines);

	std::vector<Config> data;

	for (std::list<labels_t>::iterator i = configs.begin(); i != configs.end(); i++)
	{
		std::cout << "--- server ---" << std::endl;
		labels_t &server = *i;

		Config config;

		labels_t::iterator label;
		for (label = server.begin(); label != server.end(); label++)
		{
			config.add(label->first, label->second);
		}

		data.push_back(config);
	}

	return data;
}
