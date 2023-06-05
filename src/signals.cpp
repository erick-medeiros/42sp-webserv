#include "signals.hpp"

static void signalHandler(int const sig, void *ptr)
{
	static Server *server = reinterpret_cast<Server *>(ptr);
	if (sig == SIGINT || sig == SIGQUIT)
	{
		server->~Server();
		std::cout << std::endl;
		exit(128);
	}
}

void initSignal(Server *serverInstance)
{
	void *fn = reinterpret_cast<void *>(signalHandler);
	std::signal(SIGINT, reinterpret_cast<__sighandler_t>(fn));
	std::signal(SIGQUIT, reinterpret_cast<__sighandler_t>(fn));
	signalHandler(0, serverInstance);
}
