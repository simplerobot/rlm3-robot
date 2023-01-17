#include "rlm3-debug.h"
#include "rlm3-task.h"
#include "main.h"
#include "Assert.h"


extern bool RLM3_Debug_IsActive()
{
	ASSERT(!RLM3_Task_IsISR());
	if ((ITM->TCR & ITM_TCR_ITMENA_Msk) == 0)
		return false;
	if ((ITM->TER & 1UL) == 0)
		return false;
	return true;
}

extern bool RLM3_Debug_IsActiveISR()
{
	ASSERT(RLM3_Task_IsISR());
	if ((ITM->TCR & ITM_TCR_ITMENA_Msk) == 0)
		return false;
	if ((ITM->TER & 1UL) == 0)
		return false;
	return true;
}

extern void RLM3_Debug_Output(uint8_t c)
{
	ASSERT(!RLM3_Task_IsISR());
	if ((ITM->TCR & ITM_TCR_ITMENA_Msk) == 0)
		return;
	if ((ITM->TER & 1UL) == 0)
		return;
	while (ITM->PORT[0U].u32 == 0UL)
		RLM3_Task_Yield();
	ITM->PORT[0U].u8 = c;
}

extern bool RLM3_Debug_OutputISR(uint8_t c)
{
	ASSERT(RLM3_Task_IsISR());
	if ((ITM->TCR & ITM_TCR_ITMENA_Msk) == 0)
		return false;
	if ((ITM->TER & 1UL) == 0)
		return false;
	if (ITM->PORT[0U].u32 == 0UL)
		return false;
	ITM->PORT[0U].u8 = c;
	return true;
}
