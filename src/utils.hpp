#ifndef UTILS_HPP
#define UTILS_HPP

#define BLUE "\x1b[38;5;25m"
#define CYAN "\x1b[38;5;51m"
#define YELLOW "\x1b[38;5;220m"
#define PURPLE "\x1b[38;5;162m"
#define RED "\x1b[38;5;196m"
#define GRAY "\x1b[38;5;8m"
#define GREEN "\x1b[38;5;40m"
#define RES "\x1b[0m"

#include <dirent.h>
#include <fstream> // ifstream
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

namespace utils
{
int          pathExists(const std::string &path);
int          isDir(const std::string &path);
int          isFile(const std::string &path);
std::string &trim(std::string &s);
std::string  formatSize(size_t size);
std::string  to_string(int);
bool         start_with(const std::string &text, const std::string &term);
bool         end_with(const std::string &text, const std::string &term);
std::string  readFile(const std::string &path);
} // namespace utils

#endif
