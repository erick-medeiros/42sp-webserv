#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

class Request
{
  public:
	std::string body;
};

int main()
{
	srand(time(nullptr));
	Request requests[512];

	for (int i = 0; i < 9999; ++i)
	{
		int         fdNumber = rand() % 300;
		std::string body = "Request body " + std::to_string(fdNumber);

		if (requests[fdNumber].body.empty())
		{
			requests[fdNumber].body = body;
			std::cout << "Created Request " << fdNumber << ": " << body << std::endl;
		}
		else
		{
			std::cout << "Appended to Request " << fdNumber << std::endl;
			requests[fdNumber].body += " (appended)";
		}
	}

	return 0;
}
