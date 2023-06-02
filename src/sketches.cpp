		// --- Parse config file ---
		ConfigParser config_parser(config_file);
		config_parser.parse();
		// --- Create servers ---
		std::vector<ConfigParser::Server> servers = config_parser.getServers();
		for (std::vector<ConfigParser::Server>::iterator it = servers.begin();
		     it != servers.end(); ++it)
		{
			// Create server
			Webserv::Webserv server(it->port);
			// Add routes
			std::vector<ConfigParser::Location> locations = it->locations;
			for (std::vector<ConfigParser::Location>::iterator it2 = locations.begin();
			     it2 != locations.end(); ++it2)
			{
				server.addRoute(it2->path, it2->root, it2->index, it2->methods);
			}
			// Add server to list
			this->servers.push_back(server);
		}