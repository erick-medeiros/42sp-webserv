#include "Request.hpp"

std::string &trim(std::string &s);

Request::Request(void){};

Request::Request(char *buff)
{
	std::istringstream iss(buff);

	try
	{
		init_start_line(iss);
		init_headers(iss);
		parse_URL();
	}
	catch (std::exception const &e)
	{
		throw e;
	}
};

Request::~Request(void){};

void Request::init_start_line(std::istringstream &iss)
{
	std::string row;

	std::getline(iss, row);
	if (iss.fail())
	{
		throw std::runtime_error("parsing request start-line");
	}

	std::stringstream ss(row);
	std::string       token;

	ss >> token;
	if (ss.fail())
	{
		throw std::runtime_error("parsing request method");
	}
	this->start_line["Method"] = trim(token);

	ss >> token;
	if (ss.fail())
	{
		throw std::runtime_error("parsing request resource");
	}
	this->start_line["URL"] = trim(token);

	ss >> token;
	if (ss.fail())
	{
		throw std::runtime_error("parsing request version");
	}
	this->start_line["Version"] = trim(token);
}

void Request::init_headers(std::istringstream &iss)
{
	std::string       row;
	std::string       key;
	std::string       value;
	std::stringstream ss;

	while (std::getline(iss, row))
	{
		if (iss.fail())
		{
			throw std::runtime_error("parsing request header");
		}
		if (row.size() == 1)
		{
			ss << iss.rdbuf();
			break;
		}

		key = std::string(std::strtok((char *) row.c_str(), ":"));
		value = std::string(row.begin() + row.find(" "), row.end());
		value = trim(value);

		this->header[key] = value;
	}
	if (!iss.eof())
	{
		this->body = ss.str();
	}
}

std::vector<std::string> Request::get_all_params(void)
{
	std::vector<std::string> params;
	std::stringstream        ss(this->res_query);
	std::string              token;

	while (std::getline(ss, token, '&'))
	{
		if (ss.fail())
		{
			throw std::runtime_error("parsing query parameter");
		}
		params.push_back(token);
	}

	return params;
}

void Request::parse_URL(void)
{
	std::string resource = this->start_line["URL"];

	int offset = resource.find("?");
	if (offset != -1)
	{
		this->res_path = resource.substr(0, offset);

		std::string query = resource.substr(offset + 1);
		int         query_offset = query.find("?");
		if (query_offset != -1)
		{
			query = query.substr(0, query_offset);
		}
		this->res_query = query;
	}
	else
	{
		this->res_path = resource;
		this->res_query = "";
	}
}

// debug
void Request::display_info(void)
{
	std::cout << CYAN "========== START LINE ==========" RES << std::endl;
	std::map<std::string, std::string> start_line = this->get_start_line();
	for (std::map<std::string, std::string>::const_iterator it =
	         start_line.begin();
	     it != start_line.end(); ++it)
	{
		std::cout << GRAY << it->first << RES ": " BLUE << it->second
		          << RES "\n";
	}

	std::cout << CYAN "\n===========  HEADER  ===========" RES << std::endl;
	std::map<std::string, std::string> header = this->get_header();
	for (std::map<std::string, std::string>::const_iterator it = header.begin();
	     it != header.end(); ++it)
	{
		std::cout << GRAY << it->first << RES ": " BLUE << it->second
		          << RES "\n";
	}

	std::cout << CYAN "\n============  BODY  ============" RES << std::endl;
	std::cout << GREEN << this->get_body() << RES << std::endl;

	std::cout << CYAN "\n============== EXTRA =============" RES << std::endl;
	std::vector<std::string> params = this->get_all_params();
	std::cout << GRAY "Path:   " BLUE << this->get_resource_path() << RES
	          << std::endl;
	std::cout << GRAY "Params: " BLUE << this->get_resource_query() << RES
	          << std::endl;
	for (std::vector<std::string>::const_iterator it = params.begin();
	     it != params.end(); ++it)
	{
		std::cout << YELLOW << *it << RES << std::endl;
	}
	std::cout << CYAN "==================================" RES << std::endl;
}

// getters
std::map<std::string, std::string> Request::get_start_line(void) const
{
	return this->start_line;
}

std::map<std::string, std::string> Request::get_header(void) const
{
	return this->header;
}

std::string Request::get_body(void) const
{
	return this->body;
}

std::string Request::get_resource_path(void) const
{
	return this->res_path;
}

std::string Request::get_resource_query(void) const
{
	return this->res_query;
}

std::string Request::get_method(void) const
{
	std::map<std::string, std::string>::const_iterator it =
	    this->start_line.find("Method");
	if (it == this->start_line.end())
	{
		return "";
	}
	return it->second;
};

std::string Request::get_url(void) const
{
	std::map<std::string, std::string>::const_iterator it =
	    this->start_line.find("URL");
	if (it == this->start_line.end())
	{
		return "";
	}
	return it->second;
};

std::string Request::get_header_value(std::string const header) const
{
	std::map<std::string, std::string>::const_iterator it =
	    this->start_line.find(header);
	if (it == this->start_line.end())
	{
		return "";
	}
	return it->second;
};

// utils
static std::string &rtrim(std::string &s, const char *t)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

static std::string &ltrim(std::string &s, const char *t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

std::string &trim(std::string &s)
{
	const char *ws = " \t\n\r\f\v";
	return ltrim(rtrim(s, ws), ws);
}
