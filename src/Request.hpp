#ifndef REQUEST_HPP
#define REQUEST_HPP

#define BLUE "\x1b[38;5;25m"
#define CYAN "\x1b[38;5;51m"
#define YELLOW "\x1b[38;5;220m"
#define PURPLE "\x1b[38;5;162m"
#define RED "\x1b[38;5;196m"
#define GRAY "\x1b[38;5;8m"
#define GREEN "\x1b[38;5;40m"
#define RES "\x1b[0m"

#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <vector>

class Request
{
  private:
	Request(void);
	std::map<std::string, std::string> start_line;
	std::map<std::string, std::string> header;
	std::string                        body;
	std::string                        res_path;
	std::string                        res_query;
	void                               init_start_line(std::istringstream &iss);
	void                               init_headers(std::istringstream &iss);
	void                               parse_URL(void);

  public:
	Request(char *buff);
	~Request(void);

	std::map<std::string, std::string> get_start_line(void) const;
	std::map<std::string, std::string> get_header(void) const;
	std::string                        get_body(void) const;
	std::string                        get_resource_path(void) const;
	std::string                        get_resource_query(void) const;
	std::vector<std::string>           get_all_params(void);
	std::string                        get_method(void) const;
	std::string                        get_url(void) const;
	std::string get_header_value(std::string const header) const;

	void display_info(void);
};

#endif