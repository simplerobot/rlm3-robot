#include "Control.hpp"
#include "rlm3-task.h"
#include "rlm3-motors.h"


constexpr RLM3_Time MAX_CONTROL_DELAY_MS = 100;

static RLM3_Task g_task = nullptr;
static volatile bool g_abort = false;

static volatile uint32_t g_control_time = 0;
static volatile int8_t g_control_left = 0;
static volatile int8_t g_control_right = 0;
static volatile int8_t g_control_blade = 0;


extern void Control_RunTask()
{
	RLM3_Motors_Init();

	while (!g_abort)
	{
		if (RLM3_Time_Get() - g_control_time > MAX_CONTROL_DELAY_MS)
		{
			// Disable motors.
		}

		RLM3_Task_TakeWithTimeout(50);
	}
	g_abort = false;

	RLM3_Motors_DeInit();
}

extern void Control_AbortTask()
{
	g_abort = true;
	RLM3_Task_Give(g_task);
}

extern void Control_AbortTaskISR()
{
	g_abort = true;
	RLM3_Task_GiveISR(g_task);
}

extern void CommInput_Control_Callback(uint32_t time, int8_t left, int8_t right, int8_t blade)
{
	g_control_left = left;
	g_control_right = right;
	g_control_blade = blade;
	g_control_time = time;

	RLM3_Task_Give(g_task);
}
