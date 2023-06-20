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

#include <string>

#define DEFAULT_CONF "./config/default.conf"

int  loop(std::string path_config);
bool running(bool status);

int main(int argc, char *argv[])
{
	std::string path_config = DEFAULT_CONF;

	if (argc > 1)
	{
		if (std::string(argv[1]) == "NORUN")
			running(false);
		else
			path_config = argv[1];
	}

	return loop(path_config);
}