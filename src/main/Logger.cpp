#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "Logger.h"
#include "ConsoleColor.h"
Logger::Logger()
{
}

void Logger::log(char* str)
{
	mutex.lock();
	std::cout << str;
	mutex.unlock();
}

void Logger::info(char* str)
{
	std::ostringstream ss;
	ss << green << "[" << getFormattedTimestamp() << "][INFO]: " << str << "\n";
	log(const_cast<char*>(ss.str().c_str()));
}

void Logger::info(const char* str)
{
	char* temp = const_cast<char*>(str);
	info(temp);
}

void Logger::info(std::string str)
{
	info(str.c_str());
}

void Logger::warn(char* str)
{
	std::stringstream sstm;
	sstm << yellow << "[" << getFormattedTimestamp() << "][WARN]: " << str << std::endl;
	log(const_cast<char*>(sstm.str().c_str()));
}

void Logger::warn(const char* str)
{
	char* temp = const_cast<char*>(str);
	warn(temp);
}

void Logger::warn(std::string str)
{
	warn(str.c_str());
}

void Logger::error(char* str)
{
	std::stringstream sstm;
	sstm << red << "[" << getFormattedTimestamp() << "][ERROR]: " << str << std::endl;
	log(const_cast<char*>(sstm.str().c_str()));
}

void Logger::error(const char* str)
{
	char* temp = const_cast<char*>(str);
	error(temp);
}

void Logger::error(std::string str)
{
	error(str.c_str());
}

std::string Logger::getFormattedTimestamp() const
{
	/* get seconds since the Epoch */
	time_t secs = time(0);

	/* convert to localtime */
	struct tm local;
	localtime_s(&local, &secs);

	std::stringstream ss;
	ss << std::setfill('0') << std::setw(2) << local.tm_hour << ":";
	ss << std::setfill('0') << std::setw(2) << local.tm_min << ":";
	ss << std::setfill('0') << std::setw(2) << local.tm_sec;

	return ss.str();
}
