#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"

class RequestHandler
{
  public:
	RequestHandler();
	RequestHandler(Config &config);

	Response handle(const Request &request);

  private:
	Config &_config;
};

#endif
