#include "Test.h"
#include "rlm3-id.h"
#include "logger.h"


LOGGER_ZONE(TEST);


TEST_CASE(ID_Get_HappyCase)
{
	uint8_t id[RLM3_ID_SIZE];

	for (size_t i = 0; i < RLM3_ID_SIZE; i++)
		id[i] = RLM3_ID_Get(i);

	LOG_ALWAYS("Device ID: %x %x %x %x %x %x %x %x %x %x %x %x", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7], id[8], id[9], id[10], id[11]);
}
