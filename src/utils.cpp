#include "utils.hpp"

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
