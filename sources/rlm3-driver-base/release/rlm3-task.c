#include "rlm3-task.h"
#include <stm32f4xx.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Assert.h"
#include "logger.h"


LOGGER_ZONE(TASK);


#define RLM3_TASK_NORMAL_PRIORITY (24)


extern RLM3_Time RLM3_Time_Get()
{
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());

	return xTaskGetTickCount();
}

extern RLM3_Time RLM3_Time_GetISR()
{
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());

	return xTaskGetTickCountFromISR();
}

extern bool RLM3_Task_IsSchedulerRunning()
{
	return (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);
}


extern bool RLM3_Task_IsISR()
{
	return (__get_IPSR() != 0U);
}

extern void RLM3_Task_Yield()
{
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());
	taskYIELD();
}

extern void RLM3_Task_Delay(RLM3_Time delay_ms)
{
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());
	vTaskDelay(delay_ms + 1);
}

extern void RLM3_Task_DelayUntil(RLM3_Time start_time, RLM3_Time delay_ms)
{
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());
	RLM3_Time start_time_copy = start_time;
	vTaskDelayUntil(&start_time_copy, delay_ms);
}

static void TaskMainFn(void* param)
{
	RLM3_Task_Fn fn = (RLM3_Task_Fn)param;
	fn();
	vTaskDelete(NULL);
}

extern RLM3_Task RLM3_Task_Create(RLM3_Task_Fn fn, size_t stack_size_words, const char* name)
{
	TaskHandle_t task = NULL;
	BaseType_t result = xTaskCreate(TaskMainFn, name, stack_size_words, fn, RLM3_TASK_NORMAL_PRIORITY, &task);
	if (task == NULL)
	{
		LOG_ERROR("Task Create '%s' %d Failed %d %c", name, (int)stack_size_words, (int)result, (task == NULL) ? 'N' : 'Y');
		return NULL;
	}
	return (RLM3_Task)task;
}

extern RLM3_Task RLM3_Task_GetCurrent()
{
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());
	return (RLM3_Task)xTaskGetCurrentTaskHandle();
}

extern void RLM3_Task_Give(RLM3_Task task)
{
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());
	if (task != NULL)
	{
		xTaskNotifyGive((TaskHandle_t)task);
	}
}

extern void RLM3_Task_GiveISR(RLM3_Task task)
{
	ASSERT(RLM3_Task_IsISR());
	if (task != NULL)
	{
		BaseType_t higher_priority_task_woken = pdFALSE;
		vTaskNotifyGiveFromISR((TaskHandle_t)task, &higher_priority_task_woken);
		portYIELD_FROM_ISR(higher_priority_task_woken);
	}
}

extern void RLM3_Task_Take()
{
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

extern bool RLM3_Task_TakeWithTimeout(RLM3_Time timeout_ms)
{
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());
	uint32_t value = ulTaskNotifyTake(pdTRUE, timeout_ms + 1);
	return (value > 0);
}

extern bool RLM3_Task_TakeUntil(RLM3_Time start_time, RLM3_Time delay_ms)
{
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());
	RLM3_Time current_time = xTaskGetTickCount();
	if (current_time - start_time >= delay_ms)
		return false;
	uint32_t value = ulTaskNotifyTake(pdTRUE, delay_ms + start_time - current_time);
	return (value > 0);
}

