#include "../lib/doctest.h"
#include "Request.hpp"

typedef std::map<std::string, std::string> map;

#define TEST_FD 42

TEST_CASE("Client start line request")
{
	struct s_tt
	{
		std::string request;
		int         expected;
	} tt[] = {
	    {"GET / HTTP/1.0\r\n\r\n", 3},
	    {"GET /path HTTP/1.1\r\n\r\n", 3},
	    {"GET /path? HTTP/1.1\r\n\r\n", 3},
	    {"GET /path?query_string HTTP/1.1\r\n\r\n", 3},
	    {"GET http://localhost/ HTTP/1.1\r\n\r\n", 3},
	    {"GET http://localhost:8080/ HTTP/1.1\r\n\r\n", 3},
	    {"GET HTTP/1.1\r\n\r\n", 2},
	    {"/ HTTP/1.1\r\n\r\n", 2},
	    {"GET / \r\n\r\n", 2},
	    {"GET \r\n\r\n", 1},
	    {"\r\n\r\n", 0},
	    {"GET		/ HTTP/1.0\r\n\r\n", 3},
	    {"GET	/	HTTP/1.0\r\n\r\n", 3},
	    {"GET		/	   	HTTP/1.0		\r\n\r\n", 3},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		if (tt[i].expected < 3)
		{
			Request req(TEST_FD);
			CHECK_THROWS_AS(req.parse(tt[i].request), std::runtime_error);
		}
		else
		{
			Request req(TEST_FD);
			req.parse(tt[i].request);
			map requestStartLine = req.getStartLine();

			CHECK_EQ(requestStartLine.size(), tt[i].expected);
		}
	}
}

TEST_CASE("Client request headers")
{
	struct s_tt
	{
		std::string startLine;
		std::string headers;
		int         expected;
	} tt[] = {
	    {"GET / HTTP/1.1\r\n", "\r\n", 0},
	    {"GET / HTTP/1.1\r\n", "h1: foo\r\n\r\n", 1},
	    {"GET / HTTP/1.1\r\n", "h1: foo\r\nh2: bar\r\n\r\n", 2},
	    {"GET / HTTP/1.1\r\n", "h1: foo\r\nh2: bar\r\nh3: baz\r\n\r\n", 3},
	    {"GET / HTTP/1.1\r\n", "h1:foo\r\n h2: bar\r\nh3 : baz\r\n\r\n", 3},
	    {"GET / HTTP/1.1\r\n",
	     "h1:foo	 \r\n		h2: bar\r\nh3	 	:			baz	 		"
	     "\r\n\r\n",
	     3},
	    {"GET / HTTP/1.1\r\n", "h1:\r\n\r\n", -1},
	    {"GET / HTTP/1.1\r\n", "h1\r\n\r\n", -1},
	    {"GET / HTTP/1.1\r\n", " \r\n\r\n", 0},
	    {"GET / HTTP/1.1\r\n", "      \r\n\r\n", 0},
	    {"GET / HTTP/1.1\r\n", "	\r\n\r\n", 0},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		std::string const request = tt[i].startLine + tt[i].headers;

		if (tt[i].expected == -1)
		{
			Request req(TEST_FD);
			CHECK_THROWS_AS(req.parse(request), std::runtime_error);
		}
		else
		{
			Request req(TEST_FD);
			req.parse(request);
			map requestHeaders = req.getHeaders();

			CHECK_EQ(requestHeaders.size(), tt[i].expected);
		}
	}
}

TEST_CASE("Client request body")
{
	struct s_tt
	{
		std::string startLine;
		std::string headers;
		std::string body;
		int         expected;
	} tt[] = {
	    {"GET / HTTP/1.1\r\n", "\r\n", "", 0},
	    {"GET / HTTP/1.1\r\n", "Content-Length: 1\r\n\r\n", "\n", 1},
	    {"GET / HTTP/1.1\r\n", "Content-Length: 3\r\n\r\n", "foo", 3},
	    {"GET / HTTP/1.1\r\n", "Content-Length: 11\r\n\r\n", "foo bar baz", 11},
	    {"GET / HTTP/1.1\r\n", "Content-Length: 10\r\n\r\n", "{\"k\": \"v\"}", 10},
	    {"GET / HTTP/1.1\r\n", "h1: foo\r\nContent-Length: 24\r\n\r\n",
	     "{\"k1\": \"v1\", \"k2\": \"v2\"}", 24},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		std::string const request = tt[i].startLine + tt[i].headers + tt[i].body;
		Request           req(TEST_FD);
		req.parse(request);
		std::string const requestBody = req.getBody();

		CHECK_EQ(requestBody.size(), tt[i].expected);
	}
}

TEST_CASE("getResourcePath")
{
	struct s_tt
	{
		std::string startLine;
		std::string expected;
	} tt[] = {
	    {"GET / HTTP/1.1\r\n\r\n", "/"},
	    {"GET /foo HTTP/1.1\r\n\r\n", "/foo"},
	    {"GET /foo/ HTTP/1.1\r\n\r\n", "/foo/"},
	    {"GET /foo/bar/baz HTTP/1.1\r\n\r\n", "/foo/bar/baz"},
	    {"GET /? HTTP/1.1\r\n\r\n", "/"},
	    {"GET /foo? HTTP/1.1\r\n\r\n", "/foo"},
	    {"GET /foo/bar?query HTTP/1.1\r\n\r\n", "/foo/bar"},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		std::string const request = tt[i].startLine;
		Request           req(TEST_FD);
		req.parse(request);
		std::string const requestPath = req.getResourcePath();

		CHECK_EQ(requestPath, tt[i].expected);
	}
}

TEST_CASE("getResourceQuery")
{
	struct s_tt
	{
		std::string startLine;
		std::string expected;
	} tt[] = {
	    {"GET / HTTP/1.1\r\n\r\n", ""},
	    {"GET /foo HTTP/1.1\r\n\r\n", ""},
	    {"GET /foo/ HTTP/1.1\r\n\r\n", ""},
	    {"GET /? HTTP/1.1\r\n\r\n", ""},
	    {"GET /foo? HTTP/1.1\r\n\r\n", ""},
	    {"GET /foo/bar?query HTTP/1.1\r\n\r\n", "query"},
	    {"GET /?arg1=foo&arg2=bar HTTP/1.1\r\n\r\n", "arg1=foo&arg2=bar"},
	    {"GET /foo?arg1=foo?arg2=bar HTTP/1.1\r\n\r\n", "arg1=foo"},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		std::string const request = tt[i].startLine;
		Request           req(TEST_FD);
		req.parse(request);
		std::string const requestPath = req.getResourceQuery();

		CHECK_EQ(requestPath, tt[i].expected);
	}
}

TEST_CASE("getAllParams")
{
	struct s_tt
	{
		std::string startLine;
		int         expected;
	} tt[] = {
	    {"GET / HTTP/1.1\r\n\r\n", 0},
	    {"GET /foo HTTP/1.1\r\n\r\n", 0},
	    {"GET /? HTTP/1.1\r\n\r\n", 0},
	    {"GET /foo/bar?query HTTP/1.1\r\n\r\n", 1},
	    {"GET /?arg1=foo&arg2=bar HTTP/1.1\r\n\r\n", 2},
	    {"GET /foo?arg1=foo?arg2=bar HTTP/1.1\r\n\r\n", 1},
	    {"GET /?arg1=foo&arg2=bar&arg3=baz HTTP/1.1\r\n\r\n", 3},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		std::string const request = tt[i].startLine;
		Request           req(TEST_FD);
		req.parse(request);
		std::vector<std::string> const params = req.getAllParams();

		CHECK_EQ(params.size(), tt[i].expected);
	}
}

TEST_CASE("getMethod")
{
	struct s_tt
	{
		std::string startLine;
		std::string expected;
	} tt[] = {
	    {"GET / HTTP/1.1\r\n\r\n", "GET"},
	    {"POST /foo?a=1&b=2 HTTP/1.1\r\n\r\n", "POST"},
	    {"DELETE /?id=1 HTTP/1.1\r\n\r\n", "DELETE"},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		std::string const request = tt[i].startLine;
		Request           req(TEST_FD);
		req.parse(request);
		std::string const method = req.getMethod();

		CHECK_EQ(method, tt[i].expected);
	}
}

TEST_CASE("getUrl")
{
	struct s_tt
	{
		std::string startLine;
		std::string expected;
	} tt[] = {
	    {"GET / HTTP/1.1\r\n\r\n", "/"},
	    {"POST /foo?a=1&b=2 HTTP/1.1\r\n\r\n", "/foo?a=1&b=2"},
	    {"DELETE /?id=1 HTTP/1.1\r\n\r\n", "/?id=1"},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		std::string const request = tt[i].startLine;
		Request           req(TEST_FD);
		req.parse(request);
		std::string const url = req.getUrl();

		CHECK_EQ(url, tt[i].expected);
	}
}

TEST_CASE("getHeaderValue")
{
	struct s_tt
	{
		std::string header;
		std::string value;
		std::string expected;
		bool        expectException;
	} tt[] = {
	    {"GET / HTTP/1.1\r\nkey: value\r\n\r\n", "key", "value", false},
	    {"GET / HTTP/1.1\r\nfoo: 1\r\nbar: 2\r\nbaz: 3\r\n\r\n", "foo", "1", false},
	    {"GET / HTTP/1.1\r\nfoo: 1\r\nbar: 2\r\nbaz: 3\r\n\r\n", "bar", "2", false},
	    {"GET / HTTP/1.1\r\nfoo: 1\r\nbar: 2\r\nbaz: 3\r\n\r\n", "baz", "3", false},
	    {"GET / HTTP/1.1\r\nfoo:1\r\nbar:2\r\nbaz:3\r\n\r\n", "foo", "1", false},
	    {"GET / HTTP/1.1\r\nfoo:1\r\nbar:2\r\nbaz:3\r\n\r\n", "baz", "3", false},
	    {"GET / HTTP/1.1\r\nfoo:1\r\nbar:2\r\nbaz:3\r\n\r\n", "", "", false},
	    {"GET / HTTP/1.1\r\nkey: value\r\n\r\n", "foo", "", false},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		std::string const request = tt[i].header;
		if (tt[i].expectException)
		{
			Request req(TEST_FD);
			req.parse(request);
			CHECK_THROWS_AS(req.getHeaderValue(tt[i].value), std::runtime_error);
		}
		else
		{
			Request req(TEST_FD);
			req.parse(request);
			std::string const headerValue = req.getHeaderValue(tt[i].value);

			CHECK_EQ(headerValue, tt[i].expected);
		}
	}
}

TEST_CASE("isValidMethod")
{
	struct s_tt
	{
		std::string header;
		bool        expected;
	} tt[] = {
	    {"GET / HTTP/1.1\r\n\r\n", true},
	    {"POST / HTTP/1.1\r\n\r\n", true},
	    {"DELETE / HTTP/1.1\r\n\r\n", true},
	    {"UPDATE / HTTP/1.1\r\n\r\n", false},
	    {"get / HTTP/1.1\r\n\r\n", false},
	    {"GET / HTTP/1.1\r\nh1: value1\r\n\r\n", true},
	    {"POST / HTTP/1.1\r\nh1: value1\r\n\r\n", true},
	    {"DELETE / HTTP/1.1\r\nh1: value1\r\n\r\n", true},
	    {"post / HTTP/1.1\r\nh1: value1\r\n\r\n", false},
	    {"FOO / HTTP/1.1\r\nh1: value1\r\n\r\n", false},
	    {"OPTIONS / HTTP/1.1\r\nh1: value1\r\n\r\n", false},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		std::string const request = tt[i].header;
		if (tt[i].expected == false)
		{
			Request req(TEST_FD);
			CHECK_THROWS_AS(req.parse(request), std::runtime_error);
		}
		else
		{
			Request req(TEST_FD);
			req.parse(request);
			std::string const requestMethod = req.getMethod();
			bool              isValidMethod = req.isValidMethod(requestMethod);

			CHECK_EQ(isValidMethod, tt[i].expected);
		}
	}
}

TEST_CASE("isValidHttpVersion")
{
	struct s_tt
	{
		std::string header;
		bool        expected;
	} tt[] = {
	    {"GET / HTTP/1.0\r\n\r\n", true},    {"GET / HTTP/1.1\r\n\r\n", true},
	    {"GET / HTTP/1.2\r\n\r\n", false},   {"GET / HTTP/2.0\r\n\r\n", false},
	    {"GET / HTTP/1.0.0\r\n\r\n", false}, {"GET / HTTP/1.1.0\r\n\r\n", false},
	};

	std::size_t n = sizeof(tt) / sizeof(*tt);

	std::size_t i = -1;
	while (++i < n)
	{
		std::string const request = tt[i].header;
		if (tt[i].expected == false)
		{
			Request req(TEST_FD);

			CHECK_THROWS_AS(req.parse(request), std::runtime_error);
		}
		else
		{
			Request req(TEST_FD);
			req.parse(request);
			std::string const requestMethod = req.getMethod();
			bool              isValidMethod = req.isValidMethod(requestMethod);

			CHECK_EQ(isValidMethod, tt[i].expected);
		}
	}
}
