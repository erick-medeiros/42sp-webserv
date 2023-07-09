/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 16:04:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/20 11:32:30 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include "utils.hpp"
#include <string>

#define DEFAULT_CONF "./config/default.conf"

int  loop(std::string path_config);
bool running(bool status);

int main(int argc, char *argv[])
{
	std::string path_config = DEFAULT_CONF;

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			std::string flagLogLevel = "--log-level=";
			std::string arg = argv[i];

			if (arg == "--no-run")
				running(false);
			else if (utils::start_with(arg, flagLogLevel))
				Logger::swithLevel(arg.substr(flagLogLevel.size()));
			else
				path_config = argv[i];
		}
	}

	return loop(path_config);
}