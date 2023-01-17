#include "rlm3-id.h"
#include "Test.hpp"


static const uint8_t DEFAULT_ID[12] = { 0x39, 0x00, 0x3d, 0x00, 0x11, 0x51, 0x36, 0x30, 0x34, 0x38, 0x37, 0x30 };

static uint8_t g_id[12];


extern uint8_t RLM3_ID_Get(size_t index)
{
	ASSERT(index < 12);
	return g_id[index];
}

extern void SIM_ID_Set(size_t index, uint8_t value)
{
	ASSERT(index < 12);
	g_id[index] = value;
}

TEST_SETUP(RLM3_ID_Setup)
{
	for (size_t i = 0; i < 12; i++)
		g_id[i] = DEFAULT_ID[i];
}
