#include "Test.hpp"
#include "logger.h"
#include <sstream>
#include <cstdarg>
#include <string>
#include <vector>
#include <algorithm>


LOGGER_ZONE(LOGGER_TEST);


static std::vector<std::string> g_logs;


extern void logger_format_message(LoggerLevel level, const char* zone, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	std::ostringstream out;
	out << ToString(level) << " " << zone << " ";
	while (*format != 0)
	{
		char c = *(format++);
		if (c == '%')
		{
			char f = *(format++);
			if (f == 's')
				out << va_arg(args, const char*);
			else if (f == 'd')
				out << va_arg(args, int);
			else if (f == '%')
				out.put('%');
			else
				out << "[unknown format " << f << "]";
		}
		else
			out.put(c);
	}

	va_end(args);

	g_logs.push_back(out.str());
//	std::puts(out.str().c_str());
}


TEST_CASE(logger_happy_case)
{
	g_logs.clear();

	LOG_ALWAYS("Always");
	LOG_FATAL("Fatal");
	LOG_ERROR("Error");
	LOG_WARN("Warn");
	LOG_INFO("Info");
	LOG_DEBUG("Debug");
	LOG_TRACE("Trace");

	ASSERT(g_logs.size() == 4);
	ASSERT(g_logs[0] == "ALWAYS LOGGER_TEST Always");
	ASSERT(g_logs[1] == "FATAL LOGGER_TEST Fatal");
	ASSERT(g_logs[2] == "ERROR LOGGER_TEST Error");
	ASSERT(g_logs[3] == "WARN LOGGER_TEST Warn");
}

TEST_CASE(logger_show_zones)
{
	g_logs.clear();

	logger_show_zones();

	ASSERT(std::find(g_logs.begin(), g_logs.end(), "ALWAYS LOGGER Zone 'LOGGER' log level WARN.") != g_logs.end());
	ASSERT(std::find(g_logs.begin(), g_logs.end(), "ALWAYS LOGGER Zone 'LOGGER_TEST' log level WARN.") != g_logs.end());
}
