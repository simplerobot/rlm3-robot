#include "rlm3-id.h"
#include "main.h"
#include "Assert.h"


extern uint8_t RLM3_ID_Get(size_t byte)
{
	ASSERT(byte < RLM3_ID_SIZE);
	return ((uint8_t*)UID_BASE)[byte];
}
