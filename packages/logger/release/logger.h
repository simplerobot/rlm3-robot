#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __CDT_PARSER__ // Eclipse CDT parser does not define __BASE_FILE__.
	#define __BASE_FILE__ ""
#endif

#ifndef __BASE_FILE__
	#error __BASE_FILE__ macro is not defined.
#endif


enum LoggerLevel
{
	LOGGER_LEVEL_INVALID,
	LOGGER_LEVEL_ALL,
	LOGGER_LEVEL_TRACE,
	LOGGER_LEVEL_DEBUG,
	LOGGER_LEVEL_INFO,
	LOGGER_LEVEL_WARN,
	LOGGER_LEVEL_ERROR,
	LOGGER_LEVEL_FATAL,
	LOGGER_LEVEL_ALWAYS,
	LOGGER_LEVEL_NONE,
};
typedef enum LoggerLevel LoggerLevel;

extern const char* ToString(LoggerLevel level);
extern bool Parse(LoggerLevel* level, const char* string);


#define LOG_TRACE(FORMAT, ...)		INTERNAL_LOGGER(LOGGER_LEVEL_TRACE, FORMAT, ##__VA_ARGS__)
#define LOG_DEBUG(FORMAT, ...)		INTERNAL_LOGGER(LOGGER_LEVEL_DEBUG, FORMAT, ##__VA_ARGS__)
#define LOG_INFO(FORMAT, ...)		INTERNAL_LOGGER(LOGGER_LEVEL_INFO, FORMAT, ##__VA_ARGS__)
#define LOG_WARN(FORMAT, ...) 		INTERNAL_LOGGER(LOGGER_LEVEL_WARN, FORMAT, ##__VA_ARGS__)
#define LOG_ERROR(FORMAT, ...)		INTERNAL_LOGGER(LOGGER_LEVEL_ERROR, FORMAT, ##__VA_ARGS__)
#define LOG_FATAL(FORMAT, ...)		INTERNAL_LOGGER(LOGGER_LEVEL_FATAL, FORMAT, ##__VA_ARGS__)
#define LOG_ALWAYS(FORMAT, ...)		INTERNAL_LOGGER(LOGGER_LEVEL_ALWAYS, FORMAT, ##__VA_ARGS__)

#define IS_LOG_TRACE()				INTERNAL_IS_LOGGER(LOGGER_LEVEL_TRACE)
#define IS_LOG_DEBUG()				INTERNAL_IS_LOGGER(LOGGER_LEVEL_DEBUG)
#define IS_LOG_INFO()				INTERNAL_IS_LOGGER(LOGGER_LEVEL_INFO)
#define IS_LOG_WARN()				INTERNAL_IS_LOGGER(LOGGER_LEVEL_WARN)
#define IS_LOG_ERROR()				INTERNAL_IS_LOGGER(LOGGER_LEVEL_ERROR)
#define IS_LOG_FATAL()				INTERNAL_IS_LOGGER(LOGGER_LEVEL_FATAL)


struct LoggerZone
{
	LoggerLevel level;
	const char* zone;
	struct LoggerZone* next;
};
typedef struct LoggerZone LoggerZone;


#define LOGGER_ZONE(ZONE) static LoggerZone g_logger_zone = { LOGGER_LEVEL_INVALID, NULL, NULL }; static __attribute__((constructor)) void logger_init_constructor_fn() { logger_internal_initialize_zone(&g_logger_zone, #ZONE); } extern int ensure_zone_is_a_valid_token_ ## ZONE ## _end

#define INTERNAL_IS_LOGGER(LEVEL) ((LEVEL) >= g_logger_zone.level)
#define INTERNAL_LOGGER(LEVEL, FORMAT, ...) do { if (INTERNAL_IS_LOGGER(LEVEL)) logger_format_message(LEVEL, g_logger_zone.zone, FORMAT, ##__VA_ARGS__); } while (0)


extern void logger_internal_initialize_zone(LoggerZone* zone, const char* zone_name);
extern bool logger_set_level(const char* zone_name, LoggerLevel level);
extern void logger_show_zones();


// The application must override this method.
extern void logger_format_message(LoggerLevel level, const char* zone, const char* format, ...) __attribute__ ((format (printf, 3, 4)));


#ifdef __cplusplus
}
#endif

