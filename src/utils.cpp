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

int utils::isDir(const std::string &path)
{
	DIR *dir = opendir(path.c_str());
	if (dir != NULL)
	{
		closedir(dir);
		return 1;
	}
	return 0;
}

int utils::isFile(const std::string &path)
{
	struct stat path_stat;
	stat(path.c_str(), &path_stat);
	return S_ISREG(path_stat.st_mode);
}

int utils::pathExists(const std::string &path)
{
	struct stat path_stat;
	return stat(path.c_str(), &path_stat) == 0;
}

std::string utils::formatSize(size_t size)
{
	std::stringstream ss;

	const char *units[] = {"B", "KB", "MB", "GB"};
	int         unit = 0;
	while (size > 1024 && unit < 3)
	{
		size /= 1024;
		++unit;
	}
	ss << size << units[unit];
	return ss.str();
}