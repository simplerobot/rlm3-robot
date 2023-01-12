#include "Test.h"
#include "rlm3-debug.h"


TEST_CASE(IsDebugOutput_HappyCase)
{
	ASSERT(RLM3_Debug_IsActive());
}
