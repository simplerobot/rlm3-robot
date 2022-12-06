#include "logger.h"
#include <string.h>


LOGGER_ZONE(LOGGER);


#ifndef DEFAULT_LOGGER_LEVEL
#define DEFAULT_LOGGER_LEVEL LOGGER_LEVEL_WARN
#endif


static LoggerZone* g_logger_zone_list = NULL;


extern const char* ToString(LoggerLevel level)
{
	switch (level)
	{
	case LOGGER_LEVEL_ALL:		return "ALL";
	case LOGGER_LEVEL_TRACE:	return "TRACE";
	case LOGGER_LEVEL_DEBUG:	return "DEBUG";
	case LOGGER_LEVEL_INFO:		return "INFO";
	case LOGGER_LEVEL_WARN:		return "WARN";
	case LOGGER_LEVEL_ERROR:	return "ERROR";
	case LOGGER_LEVEL_FATAL:	return "FATAL";
	case LOGGER_LEVEL_NONE:		return "NONE";
	case LOGGER_LEVEL_ALWAYS:	return "ALWAYS";
	default:					return "<invalid>";
	}
}

extern bool Parse(LoggerLevel* level, const char* string)
{
	for (int i = LOGGER_LEVEL_ALL; i <= LOGGER_LEVEL_NONE; i++)
	{
		if (strcmp(string, ToString((LoggerLevel)i)) == 0)
		{
			if (level != NULL)
				*level = (LoggerLevel)i;
			return true;
		}
	}
	if (level != NULL)
		*level = LOGGER_LEVEL_INVALID;
	return false;
}

extern void logger_internal_initialize_zone(LoggerZone* zone, const char* zone_name)
{
	zone->level = DEFAULT_LOGGER_LEVEL;
	zone->zone = zone_name;
	zone->next = g_logger_zone_list;
	g_logger_zone_list = zone;
}

extern bool logger_set_level(const char* zone_name, LoggerLevel level)
{
	bool result = false;
	for (LoggerZone* cursor = g_logger_zone_list; cursor != NULL; cursor = cursor->next)
	{
		if (strcmp(zone_name, cursor->zone) == 0)
		{
			cursor->level = level;
			LOG_ALWAYS("Set zone '%s' log level to %s.", zone_name, ToString(level));
			result = true;
		}
	}

	if (!result)
		LOG_ALWAYS("Zone '%s' not found.", zone_name);
	return result;
}

extern void logger_show_zones()
{
	for (LoggerZone* cursor = g_logger_zone_list; cursor != NULL; cursor = cursor->next)
		LOG_ALWAYS("Zone '%s' log level %s.", cursor->zone, ToString(cursor->level));
}

