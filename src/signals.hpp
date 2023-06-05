#ifndef SIGNALS_HPP
#define SIGNALS_HPP

#include <csignal>
#include <iostream>

#include "Server.hpp"

class Server;

void initSignal(Server *serverInstance);

#endif
