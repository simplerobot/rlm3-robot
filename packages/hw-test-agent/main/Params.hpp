#pragma once

#include <cstdint>
#include <iostream>


struct Parameters
{
	bool invalid_parameters = false;
	bool run_test = false;
	bool run_server = false;
	bool show_config = false;
	bool show_help = false;
	uint32_t lock_timeout_ms = 0;
	uint32_t test_timeout_ms = 0;
	uint32_t system_frequency_hz = 0;
	uint32_t trace_frequency_hz = 0;
	const char* service = nullptr;
	const char* board = nullptr;
	const char* file = nullptr;
};

extern std::ostream& operator<<(std::ostream& out, const Parameters& params);
extern std::string ToString(const Parameters& params);

extern Parameters ParseCommandLineArguments(int argc, char* const argv[]);
extern void PrintHelp(const char* app_name);

extern uint32_t ParseFrequencyParameter(Parameters& params, const char* text);
