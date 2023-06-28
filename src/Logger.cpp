/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 13:59:06 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 23:17:33 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

Logger::Logger() : _stream(std::cout), _level(LOGGER_LEVEL_INFO), _color(true) {}

Logger::Logger(std::ostream &stream)
    : _stream(stream), _level(LOGGER_LEVEL_INFO), _color(true)
{
}

Logger::~Logger() {}

void Logger::level(e_log_level level)
{
	_level = level;
}

void Logger::debug(std::string msg)
{
	if (_level > LOGGER_LEVEL_DEBUG)
		return;
	if (_color)
		_stream << LOGGER_COLOR_DEBUG;
	_stream << msg;
	if (_color)
		_stream << LOGGER_COLOR_NONE;
	_stream << std::endl;
}

void Logger::info(std::string msg)
{
	if (_level > LOGGER_LEVEL_INFO)
		return;
	if (_color)
		_stream << LOGGER_COLOR_INFO;
	_stream << msg;
	if (_color)
		_stream << LOGGER_COLOR_NONE;
	_stream << std::endl;
}

void Logger::warning(std::string msg)
{
	if (_level > LOGGER_LEVEL_WARNING)
		return;
	if (_color)
		_stream << LOGGER_COLOR_WARNING;
	_stream << msg;
	if (_color)
		_stream << LOGGER_COLOR_NONE;
	_stream << std::endl;
}

void Logger::error(std::string msg)
{
	if (_level > LOGGER_LEVEL_ERROR)
		return;
	if (_color)
		_stream << LOGGER_COLOR_ERROR;
	_stream << msg;
	if (_color)
		_stream << LOGGER_COLOR_NONE;
	_stream << std::endl;
}
