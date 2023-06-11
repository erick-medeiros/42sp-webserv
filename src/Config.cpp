/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/11 00:09:33 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(void) {}

Config::~Config(void) {}

int Config::add(string label, string value)
{
	if (label == "port")
		return _setPorts(value);
	if (label == "server_name")
		return _setServerName(value);
	if (label == "error_page")
		return _setErrorPage(value);
	if (label == "client_body_size")
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
	logError("config label not match: ", label);
	return 1;
}

bool Config::isValid(void) const
{
	if (_ports.size() == 0)
		return false;
	return true;
}

int Config::_setPorts(string &value)
{
	stringstream ss(value);
	int          port;

	if (_ports.size() > 0)
	{
		logError("port: exist");
		return FAILURE;
	}

	while (!ss.eof())
	{
		ss >> port;
		if (port < 1024 || port > 49151)
		{
			logError("port: range invalid");
			return FAILURE;
		}
		_ports.push_back(port);
	}

	return ss.fail();
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
	_errorPage[error] = page;
	return 0;
}

int Config::_setClientBodySize(string &value)
{
	stringstream ss(value);
	ss >> _clientBodySize;
	if (_clientBodySize == 0)
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

int Config::_setResponseIsDir(string &value)
{
	if (_locations.size() == 0)
		return 1;
	stringstream                  ss(value);
	vector<location_t>::reference location = _locations.back();

	location.response_is_dir = value;

	return 0;
}

vector<int> const &Config::getPorts(void) const
{
	return _ports;
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
		logError("Error: readFile");
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
			config.add(label->first, label->second);
		}

		data.push_back(config);
	}

	return data;
}
