/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 13:59:06 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/04 15:38:04 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

e_log_level Logger::level = LOGGER_LEVEL_INFO;
bool        Logger::showColor = true;

Logger::Logger() : _stream(std::cout) {}

Logger::Logger(std::ostream &stream) : _stream(stream) {}

Logger::~Logger() {}

bool Logger::_checkLevel(e_log_level type)
{
	_type = type;
	if (level > type)
		return true;
	return false;
}

const std::string Logger::_start() const
{
	if (showColor)
	{
		switch (_type)
		{
		case LOGGER_LEVEL_DEBUG:
			return LOGGER_COLOR_DEBUG;
		case LOGGER_LEVEL_INFO:
			return LOGGER_COLOR_INFO;
		case LOGGER_LEVEL_WARNING:
			return LOGGER_COLOR_WARNING;
		case LOGGER_LEVEL_ERROR:
			return LOGGER_COLOR_ERROR;
		case LOGGER_LEVEL_NONE:
			return LOGGER_COLOR_NONE;
		}
	}
	return "";
}

std::ostream &Logger::_end(std::ostream &os)
{
	if (showColor)
		os << LOGGER_COLOR_NONE;
	os << std::endl;
	return os;
}

void Logger::debug(std::string msg)
{
	if (_checkLevel(LOGGER_LEVEL_DEBUG))
		return;
	_stream << _start() << msg << _end;
}

void Logger::info(std::string msg)
{
	if (_checkLevel(LOGGER_LEVEL_INFO))
		return;
	_stream << _start() << msg << _end;
}

void Logger::warning(std::string msg)
{
	if (_checkLevel(LOGGER_LEVEL_WARNING))
		return;
	_stream << _start() << msg << _end;
}

void Logger::error(std::string msg)
{
	if (_checkLevel(LOGGER_LEVEL_ERROR))
		return;
	_stream << _start() << msg << _end;
}

void Logger::error(std::string msg, int n)
{
	if (_checkLevel(LOGGER_LEVEL_ERROR))
		return;
	_stream << _start() << msg << ": " << utils::to_string(n) << _end;
}

void Logger::error(std::string msg, char *s)
{
	if (_checkLevel(LOGGER_LEVEL_ERROR))
		return;
	_stream << _start() << msg << ": " << std::string(s) << _end;
}
