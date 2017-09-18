#pragma once
#include <mutex>

class Logger
{
private:
	std::mutex mutex;

	/**
	 * Print a string to the console without breaking the line.
	 * This is thread-safe
	 */
	void log(char* str);

public:

	Logger();
	/**
	 * Log info to the console without a line brek.
	 */
	void info(char* str);
	void info(const char* str);
	void info(std::string str);
	/**
	 * Log a warning to the console without a line break.
	 */
	void warn(char* str);
	void warn(const char* str);
	void warn(std::string str);
	/**
	 * Log an error to the console without a line break.
	 */
	void error(char* str);
	void error(const char* str);
	void error(std::string str);

	std::string getFormattedTimestamp() const;
};

