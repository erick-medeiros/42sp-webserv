/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 13:59:08 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 22:21:27 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <ostream>

enum e_log_level
{
	LOGGER_LEVEL_DEBUG,
	LOGGER_LEVEL_INFO,
	LOGGER_LEVEL_WARNING,
	LOGGER_LEVEL_ERROR,
	LOGGER_LEVEL_NONE
};

class Logger
{
  private:
	static e_log_level _level;

  public:
	static std::ostream &_stream;

	static void level(e_log_level level);

	static void debug(std::string msg);
	static void info(std::string msg);
	static void warning(std::string msg);
	static void error(std::string msg);
};

#endif /* LOGGER_HPP */