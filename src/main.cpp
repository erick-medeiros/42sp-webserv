/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 16:04:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/10 12:35:15 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Server.hpp"

void execServerInstance(int argc, char **argv)
{
	Server webserv;

	webserv.init(argc, argv);
	webserv.run();
}

int main(int argc, char *argv[])
{
	if (argc > 1 &&
	    std::string(argv[1]) == "NORUN") // TODO: Improve this, used by leaks test
		return 0;

	if (argc > 1)
	{
		string file = Config::readFile(argv[1]);
		Config::parseConfig(file);
		return 0;
	}

	int numberOfServers = 1;
	for (int i = 0; i < numberOfServers; ++i)
	{
		execServerInstance(argc, argv);
	}
}