#include "Control.hpp"
#include "rlm3-task.h"
#include "rlm3-motors.h"
#include "rlm3-lock.h"


constexpr RLM3_Time MAX_CONTROL_DELAY_MS = 100;

static RLM3_Task g_task = nullptr;
static volatile bool g_abort = false;

static volatile uint32_t g_control_time = 0;
static volatile float g_control_left = 0;
static volatile float g_control_right = 0;
static volatile float g_control_blade = 0;


extern void Control_RunTask()
{
	bool is_active = false;
	g_task = RLM3_Task_GetCurrent();

	RLM3_Motors_Init();

	while (!g_abort)
	{
		uint32_t current_time = RLM3_Time_Get();

		RLM3_Lock_EnterCritical();
		uint32_t control_time = g_control_time;
		float control_left = g_control_left;
		float control_right = g_control_right;
		float control_blade = g_control_blade;
		RLM3_Lock_ExitCritical();

		if (current_time - control_time < MAX_CONTROL_DELAY_MS)
		{
			// Make sure motors are active.
			if (!is_active)
			{
				RLM3_Motors_Enable();
				is_active = true;
			}

			// Move.
			RLM3_Motors_SetWheels(control_left, control_right);
			RLM3_Motors_SetBlade(control_blade);

			// Wait up to the cutoff time for another command.
			RLM3_Time delay = MAX_CONTROL_DELAY_MS - (current_time - control_time);
			RLM3_Task_TakeUntil(current_time, delay);
		}
		else
		{
			// Make sure motors are disabled.
			if (is_active)
			{
				RLM3_Motors_Disable();
				is_active = false;
			}

			// Make doubly sure that that the motors stay disabled.
			if (control_left != 0 || control_right != 0 || control_blade != 0)
			{
				RLM3_Lock_EnterCritical();
				if (control_time != g_control_time)
				{
					g_control_left = 0;
					g_control_right = 0;
					g_control_blade = 0;
				}
				RLM3_Lock_ExitCritical();
			}

			// Wait for a new command to come in.
			RLM3_Task_Take();
		}
	}
	g_abort = false;

	if (is_active)
		RLM3_Motors_Disable();
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

extern void CommInput_Control_Callback(uint32_t time, float left, float right, float blade)
{
	RLM3_Lock_EnterCritical();
	g_control_left = left;
	g_control_right = right;
	g_control_blade = blade;
	g_control_time = time;
	RLM3_Lock_ExitCritical();

	RLM3_Task_Give(g_task);
}
