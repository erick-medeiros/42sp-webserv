/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/10 17:50:21 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(void) {}

Config::~Config(void) {}

int Config::add(string label, string value)
{
	if (label == "port")
		return _setPorts(value);
	return 1;
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

vector<int> const &Config::getPorts(void) const
{
	return _ports;
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
			cout << label->first << " : " << label->second << endl;

			config.add(label->first, label->second);
		}

		data.push_back(config);
	}

	return data;
}
