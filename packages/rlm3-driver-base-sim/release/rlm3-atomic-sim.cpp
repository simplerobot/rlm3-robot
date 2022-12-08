#include "rlm3-atomic.h"
#include "rlm3-sim.hpp"
#include "Assert.h"


// Sim does not support multiple threads, so synchronization is easy.

#define taskENTER_CRITICAL()
#define taskEXIT_CRITICAL()


extern bool RLM3_Atomic_SetBool(volatile bool* value)
{
	ASSERT(!SIM_RLM3_Is_IRQ());

	bool old_value;

	taskENTER_CRITICAL();
	old_value = *value;
	*value = true;
	taskEXIT_CRITICAL();

	return old_value;
}

extern uint8_t RLM3_Atomic_Inc8(volatile uint8_t* value)
{
	ASSERT(!SIM_RLM3_Is_IRQ());

	uint8_t new_value;

	taskENTER_CRITICAL();
	new_value = ++(*value);
	taskEXIT_CRITICAL();

	return new_value;
}

extern uint8_t RLM3_Atomic_Dec8(volatile uint8_t* value)
{
	ASSERT(!SIM_RLM3_Is_IRQ());

	uint8_t new_value;

	taskENTER_CRITICAL();
	new_value = --(*value);
	taskEXIT_CRITICAL();

	return new_value;
}

extern uint16_t RLM3_Atomic_Inc16(volatile uint16_t* value)
{
	ASSERT(!SIM_RLM3_Is_IRQ());

	uint16_t new_value;

	taskENTER_CRITICAL();
	new_value = ++(*value);
	taskEXIT_CRITICAL();

	return new_value;
}

extern uint16_t RLM3_Atomic_Dec16(volatile uint16_t* value)
{
	ASSERT(!SIM_RLM3_Is_IRQ());

	uint16_t new_value;

	taskENTER_CRITICAL();
	new_value = --(*value);
	taskEXIT_CRITICAL();

	return new_value;
}

extern uint32_t RLM3_Atomic_Inc32(volatile uint32_t* value)
{
	ASSERT(!SIM_RLM3_Is_IRQ());

	uint32_t new_value;

	taskENTER_CRITICAL();
	new_value = ++(*value);
	taskEXIT_CRITICAL();

	return new_value;
}

extern uint32_t RLM3_Atomic_Dec32(volatile uint32_t* value)
{
	ASSERT(!SIM_RLM3_Is_IRQ());

	uint32_t new_value;

	taskENTER_CRITICAL();
	new_value = --(*value);
	taskEXIT_CRITICAL();

	return new_value;
}

