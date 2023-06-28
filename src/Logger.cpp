/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 13:59:06 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 22:22:05 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include <cstdarg>
#include <iostream>
#include <ostream>

e_log_level   Logger::_level = LOGGER_LEVEL_INFO;
std::ostream &Logger::_stream = std::cout;

void Logger::level(e_log_level level)
{
	_level = level;
}

void Logger::debug(std::string msg)
{
	if (_level > LOGGER_LEVEL_DEBUG)
		return;
	Logger::_stream << msg << std::endl;
}

void Logger::info(std::string msg)
{
	if (_level > LOGGER_LEVEL_INFO)
		return;
	Logger::_stream << msg << std::endl;
}

void Logger::warning(std::string msg)
{
	if (_level > LOGGER_LEVEL_WARNING)
		return;
	Logger::_stream << msg << std::endl;
}

void Logger::error(std::string msg)
{
	if (_level > LOGGER_LEVEL_ERROR)
		return;
	Logger::_stream << msg << std::endl;
}
