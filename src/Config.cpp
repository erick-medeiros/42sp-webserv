/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/23 11:24:33 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(void) : _port(0) {}

Config::~Config(void) {}

int Config::add(string label, string value)
{
	if (label == "port")
		return _setPort(value);
	if (label == "server_name")
		return _setServerName(value);
	if (label == "error_page")
		return _setErrorPage(value);
	if (label == "client_max_body_size")
		return _setClientBodySize(value);
	if (label == "location")
		return _setLocation(value);
	if (label == "location_http_methods")
		return _setHttpMethods(value);
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
	logError("config label not match: ", label);
	return 1;
}

bool Config::isValid(void) const
{
	if (_port == 0)
		return false;
	return true;
}

int Config::_setPort(string &value)
{
	stringstream ss(value);
	int          port;

	if (_port != 0)
	{
		logError("port: exist");
		return FAILURE;
	}

	ss >> port;

	if (port < 1024 || port > 49151)
	{
		logError("port: range invalid");
		return FAILURE;
	}

	if (!ss.eof())
	{
		logError("port: error value");
		return FAILURE;
	}

	_port = port;

	return 0;
}

int Config::_setServerName(string &value)
{
	if (_serverNames.size() > 0)
	{
		logError("server_name: exist");
		return FAILURE;
	}
	if (value == "")
	{
		logError("server_name empty");
		return FAILURE;
	}

	stringstream ss(value);
	string       server_name;

	while (!ss.eof())
	{
		ss >> server_name;
		// TODO: Checar se a string contem caracteres validos para um server_name
		_serverNames.push_back(server_name);
	}

	return 0;
}

int Config::_setErrorPage(string &value)
{
	stringstream ss(value);
	int          error = 0;
	string       page;
	ss >> error;
	ss >> page;
	if (!ss.eof())
		return FAILURE;

	if (error < 400 || error > 599)
	{
		logError("error_page: range err: ", value);
		return FAILURE;
	}
	_errorPage[error] = page;
	return 0;
}

int Config::_setClientBodySize(string &value)
{
	stringstream ss(value);
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

int Config::_setLocation(string &value)
{
	location_t location;
	location.location = value;
	_locations.push_back(location);
	return 0;
}

int Config::_setHttpMethods(string &value)
{
	if (_locations.size() == 0)
		return 1;

	stringstream                  ss(value);
	vector<location_t>::reference location = _locations.back();

	while (!ss.eof())
	{
		string str;
		ss >> str;
		str = trim(str);
		if (str == "GET" || str == "POST" || str == "DELETE")
			location.http_methods.push_back(str);
		else
			return 1;
	}

	return 0;
}

int Config::_setHttpRedirection(string &value)
{
	if (_locations.size() == 0)
		return 1;
	stringstream                  ss(value);
	vector<location_t>::reference location = _locations.back();

	location.http_redirection = value;

	return 0;
}

int Config::_setRoot(string &value)
{
	if (_locations.size() == 0)
		return 1;
	stringstream                  ss(value);
	vector<location_t>::reference location = _locations.back();

	location.root = value;

	return 0;
}

int Config::_setDirectoryListing(string &value)
{
	if (_locations.size() == 0)
		return 1;
	stringstream                  ss(value);
	vector<location_t>::reference location = _locations.back();

	location.directory_listing = value;

	return 0;
}

bool Config::directoryListingEnabled(string path) const
{
	vector<location_t>::const_iterator it;
	for (it = _locations.begin(); it != _locations.end(); ++it)
	{
		if (it->location == path)
			return it->directory_listing == "on";
	}
	return false;
}

int Config::_setResponseIsDir(string &value)
{
	if (_locations.size() == 0)
		return 1;
	stringstream                  ss(value);
	vector<location_t>::reference location = _locations.back();

	location.response_is_dir = value;

	return 0;
}

int Config::_setCGI(string &value)
{
	stringstream ss(value);
	cgi_t        cgi;

	ss >> cgi.path;

	if (!ss.eof())
		return 1;

	vector<location_t>::reference location = _locations.back();

	location.cgi = cgi;

	return 0;
}

uint_t const &Config::getPort(void) const
{
	return _port;
}

vector<string> const &Config::getServerNames(void) const
{
	return _serverNames;
}

size_t const &Config::getClientBodySize() const
{
	return _clientBodySize;
}

string const &Config::getErrorPage(int error)
{
	return _errorPage[error];
}

map<int, string> const &Config::getErrorPages() const
{
	return _errorPage;
}

vector<location_t> const &Config::getLocations() const
{
	return _locations;
}

string Config::readFile(const string &filename)
{
	ifstream     file(filename.c_str());
	stringstream buffer;

	if (file)
	{
		buffer << file.rdbuf();
		file.close();
	}
	else
	{
		logError("Error: readFile", filename);
		return "";
	}

	return buffer.str();
}

static bool startWith(const string &text, const string &term)
{
	return text.substr(0, term.size()) == term;
}

static bool endWith(const string &text, const string &term)
{
	return text.substr(text.size() - term.size(), text.size()) == term;
}

static void _panic(string err)
{
	throw runtime_error("Panic: " + err);
}

static list<string> _getLineFromFileData(string &filedata)
{
	list<string> lines;
	stringstream ss(filedata);

	string line;
	while (getline(ss, line))
	{
		line = trim(line);
		if (line.size() > 0)
		{
			if (line.size() > 1 && endWith(line, "{"))
			{
				line.resize(line.size() - 1);
				line = trim(line);
				lines.push_back(line);
				lines.push_back("{");
				continue;
			}
			lines.push_back(line);
		}
	}
	return lines;
}

static pair<string, string> getLabel(string &line)
{
	pair<string, string> content;
	istringstream        ss(line);
	ss >> content.first;
	content.second = line.substr(content.first.size());
	content.second = trim(content.second);
	return content;
}

static string extractFront(list<string> &lines)
{
	string str = lines.front();
	lines.pop_front();
	return str;
}

static list<labels_t> _getLabels(list<string> &lines)
{
	list<labels_t> configs;

	bool populateServer = false;

	labels_t config;

	while (!lines.empty())
	{
		if (!populateServer)
		{
			if (extractFront(lines) != "server")
				_panic("1");
			if (extractFront(lines) != "{")
				_panic("2");
			populateServer = true;
			continue;
		}

		string line = extractFront(lines);

		if (populateServer && startWith(line, "location"))
		{
			config.push_back(getLabel(line));
			if (extractFront(lines) != "{")
				_panic("3");
			while (!lines.empty())
			{
				string line = extractFront(lines);
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

vector<Config> Config::parseConfig(string &filedata)
{
	list<string> lines = _getLineFromFileData(filedata);

	list<labels_t> configs = _getLabels(lines);

	vector<Config> data;

	for (list<labels_t>::iterator i = configs.begin(); i != configs.end(); i++)
	{
		cout << "--- server ---" << endl;
		labels_t &server = *i;

		Config config;

		for (labels_t::iterator label = server.begin(); label != server.end();
		     label++)
		{
			// cout << "label : " << label->first << " - value : " << label->second
			//      << endl;
			config.add(label->first, label->second);
		}

		data.push_back(config);
	}

	return data;
}
