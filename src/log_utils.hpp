#include <iostream>

// Log Error ( RED )
template <typename T> void logError(const T &logMessage)
{
	std::cerr << RED << logMessage << RES << std::endl;
}
template <typename T1, typename T2>
void logError(const T1 &logMessage, const T2 &additionalInfo)
{
	std::cerr << RED << logMessage << ": " << additionalInfo << RES << std::endl;
}

// Log Warning ( YELLOW )
template <typename T> void logWarning(const T &logMessage)
{
	std::cout << YELLOW << logMessage << RES << std::endl;
}
template <typename T1, typename T2>
void logWarning(const T1 &logMessage, const T2 &additionalInfo)
{
	std::cout << YELLOW << logMessage << ": " << additionalInfo << RES << std::endl;
}

// Log Info ( GRAY )
template <typename T> void logInfo(const T &logMeggase)
{
	std::cout << GRAY << logMeggase << RES << std::endl;
}
template <typename T1, typename T2>
void logInfo(const T1 &logMeggase, const T2 &additionalInfo)
{
	std::cout << GRAY << logMeggase << ": " << additionalInfo << RES << std::endl;
}

// Log Notice ( BLUE )
template <typename T> void logNotice(const T &logMessage)
{
	std::cout << BLUE << logMessage << RES << std::endl;
}
template <typename T1, typename T2>
void logNotice(const T1 &logMessage, const T2 &additionalInfo)
{
	std::cout << BLUE << logMessage << ": " << additionalInfo << RES << std::endl;
}

// Log Success ( GREEN )
template <typename T> void logSuccess(const T &logMessage)
{
	std::cout << GREEN << logMessage << RES << std::endl;
}
template <typename T1, typename T2>
void logSuccess(const T1 &logMessage, const T2 &additionalInfo)
{
	std::cout << GREEN << logMessage << ": " << additionalInfo << RES << std::endl;
}