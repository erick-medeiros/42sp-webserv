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

std::string &utils::trim(std::string &s)
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
	if (stat(path.c_str(), &path_stat))
		return 0;
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

std::string utils::to_string(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

bool utils::start_with(const std::string &text, const std::string &term)
{
	return text.substr(0, term.size()) == term;
}

bool utils::end_with(const std::string &text, const std::string &term)
{
	return text.substr(text.size() - term.size(), text.size()) == term;
}

std::string utils::readFile(const std::string &path)
{
	std::ifstream     file;
	std::stringstream buffer;

	file.open(path.c_str());
	buffer << file.rdbuf();
	file.close();

	return buffer.str();
}