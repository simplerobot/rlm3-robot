#include "main.h"
#include <cmsis_os.h>
#include "task.h"
#include "logger.h"
#include <stdarg.h>
#include <ctype.h>


LOGGER_ZONE(TEST);


static void ITM_SendString(const char* str);


extern void RLM3_Main()
{
	LOG_ALWAYS("Running tests...");
	LOG_ALWAYS("Passed!");
	ITM_SendString("EOT PASS\n");

	for (;;)
	{
		HAL_GPIO_WritePin(STATUS_LIGHT_GPIO_Port, STATUS_LIGHT_Pin, GPIO_PIN_SET);
		vTaskDelay(1000);
		HAL_GPIO_WritePin(STATUS_LIGHT_GPIO_Port, STATUS_LIGHT_Pin, GPIO_PIN_RESET);
		vTaskDelay(1000);

		LOG_ALWAYS("Still Going...");
	}
}


static void ITM_SendCharSafe(char c)
{
	if (isprint(c))
		ITM_SendChar(c);
	else
		ITM_SendChar('?');
}

static void ITM_SendString(const char* str)
{
  if (str == NULL)
	  str = "(null)";
  while (*str != 0)
	  ITM_SendCharSafe(*(str++));
}

static void ITM_SendUInt(uint32_t x)
{
  char buffer[10];
  int count = 0;
  do
  {
    buffer[count++] = '0' + (x % 10);
    x /= 10;
  } while (x != 0);
  for (int i = 0; i < count; i++)
    ITM_SendChar(buffer[count - i - 1]);
}

static void ITM_SendInt(int32_t x)
{
  if (x < 0)
  {
    ITM_SendChar('-');
    x = -x;
  }
  ITM_SendUInt((uint32_t)x);
}

static void ITM_SendHex(uint32_t x)
{
  const char* HEX_DIGITS = "0123456789abcdef";
  size_t i = 1;
  while (i < 8 && ((x >> (32 - 4 * i)) & 0x0F) == 0)
    i++;
  for (; i <= 8; i++)
    ITM_SendChar(HEX_DIGITS[(x >> (32 - 4 * i)) & 0x0F]);
}

extern void logger_format_message(LoggerLevel level, const char* zone, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	ITM_SendString(ToString(level));
	ITM_SendChar(' ');
	ITM_SendString(zone);
	ITM_SendChar(' ');

	while (*format != 0)
	{
		char c = *(format++);
		if (c == '%')
		{
			char f = *(format++);
			if (f == 's')
				ITM_SendString(va_arg(args, const char*));
			else if (f == 'd')
				ITM_SendInt(va_arg(args, int));
			else if (f == 'u')
				ITM_SendUInt(va_arg(args, unsigned int));
			else if (f == 'x')
				ITM_SendHex(va_arg(args, unsigned int));
			else if (f == 'c')
				ITM_SendCharSafe((char)va_arg(args, int));
			else if (f == '%')
				ITM_SendChar(f);
			else
			{
				ITM_SendString("[unknown format ");
				ITM_SendCharSafe(f);
				ITM_SendChar(']');
				break;
			}
		}
		else
			ITM_SendCharSafe(c);
	}
	ITM_SendChar('\n');

	va_end(args);
}

