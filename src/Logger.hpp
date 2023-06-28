/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 13:59:08 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 23:25:07 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "utils.hpp"
#include <iostream>
#include <ostream>

enum e_log_level
{
	LOGGER_LEVEL_DEBUG,
	LOGGER_LEVEL_INFO,
	LOGGER_LEVEL_WARNING,
	LOGGER_LEVEL_ERROR,
	LOGGER_LEVEL_NONE
};

#define LOGGER_COLOR_DEBUG GRAY
#define LOGGER_COLOR_INFO CYAN
#define LOGGER_COLOR_WARNING YELLOW
#define LOGGER_COLOR_ERROR RED
#define LOGGER_COLOR_NONE RES

class Logger
{
  private:
	std::ostream &_stream;
	e_log_level   _level;
	bool          _color;

  public:
	Logger();
	Logger(std::ostream &);
	~Logger();
	void level(e_log_level level);
	void debug(std::string msg);
	void info(std::string msg);
	void warning(std::string msg);
	void error(std::string msg);
};

// global
static Logger logger;

#endif /* LOGGER_HPP */