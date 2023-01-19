#include "rlm3-motors.h"
#include "Test.hpp"


static bool g_is_initialized = false;
static bool g_is_enabled = false;

static float g_left = 0.0f;
static float g_right = 0.0f;
static float g_blade = 0.0f;


extern void RLM3_Motors_Init()
{
	ASSERT(!g_is_initialized);
	ASSERT(!g_is_enabled);

	g_is_initialized = true;
}

extern void RLM3_Motors_DeInit()
{
	ASSERT(g_is_initialized);
	ASSERT(!g_is_enabled);

	g_is_initialized = false;
}

extern bool RLM3_Motors_IsInit()
{
	return g_is_initialized;
}

extern void RLM3_Motors_Enable()
{
	ASSERT(g_is_initialized);
	ASSERT(!g_is_enabled);

	g_is_enabled = true;
}

extern void RLM3_Motors_Disable()
{
	ASSERT(g_is_initialized);
	ASSERT(g_is_enabled);

	g_left = 0.0f;
	g_right = 0.0f;
	g_blade = 0.0f;
	g_is_enabled = false;
}

extern bool RLM3_Motors_IsEnabled()
{
	ASSERT(g_is_initialized);
	return g_is_enabled;
}

extern void RLM3_Motors_SetWheels(float left, float right)
{
	ASSERT(g_is_initialized);
	ASSERT(g_is_enabled);
	ASSERT(-1.0f <= left && left <= 1.0f);
	ASSERT(-1.0f <= right && right <= 1.0f);

	g_left = left;
	g_right = right;
}

extern void RLM3_Motors_SetBlade(float blade)
{
	ASSERT(g_is_initialized);
	ASSERT(g_is_enabled);
	ASSERT(-1.0f <= blade && blade <= 1.0f);

	g_blade = blade;
}

extern float SIM_Motors_GetLeft()
{
	return g_left;
}

extern float SIM_Motors_GetRight()
{
	return g_right;
}

extern float SIM_Motors_GetBlade()
{
	return g_blade;
}

TEST_SETUP(SIM_Motors_Setup)
{
	g_is_initialized = false;
	g_is_enabled = false;
	g_left = 0.0f;
	g_right = 0.0f;
	g_blade = 0.0f;
}
