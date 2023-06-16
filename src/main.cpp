/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 16:04:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/16 11:04:20 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#define DEFAULT_CONF "./config/default.conf"

int loop(std::string path_config);

int main(int argc, char *argv[])
{
	if (argc > 1 &&
	    std::string(argv[1]) == "NORUN") // TODO: Improve this, used by leaks test
		return 0;

	std::string path_config;

	if (argc > 1)
		path_config = argv[1];
	else
		path_config = DEFAULT_CONF;

	return loop(path_config);
}