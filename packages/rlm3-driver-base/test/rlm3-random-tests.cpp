#include "Test.h"
#include "rlm3-random.h"
#include "logger.h"
#include "rlm3-task.h"
#include <algorithm>
#include <numeric>


LOGGER_ZONE(TEST_RANDOM);


typedef void (*EntropyHandlerFn)(uint32_t entropy);
static EntropyHandlerFn g_handler_fn = NULL;


TEST_CASE(Random_Lifecycle)
{
	ASSERT(!RLM3_Random_IsInit());
	RLM3_Random_Init();
	ASSERT(RLM3_Random_IsInit());
	RLM3_Random_Deinit();
	ASSERT(!RLM3_Random_IsInit());
}

TEST_CASE(Random_Get_Frequency)
{
	static size_t g_count = 0;
	g_handler_fn = [](uint32_t entropy) { g_count++; };

	RLM3_Random_Init();
	RLM3_Task_Delay(100);
	RLM3_Random_Deinit();

	ASSERT(g_count > 10000);
}

TEST_CASE(Random_Get_Distribution)
{
	static size_t* g_counts;
	g_counts = (size_t*)calloc(256, sizeof(size_t));

	g_handler_fn = [](uint32_t entropy)
	{
		for (size_t i = 0; i < 32; i += 8)
			g_counts[(entropy >> i) % 256]++;
	};

	RLM3_Random_Init();
	RLM3_Task_Delay(100);
	RLM3_Random_Deinit();

	size_t total = std::accumulate(g_counts, g_counts + 256, 0);
	size_t min = *std::min_element(g_counts, g_counts + 256);
	size_t max = *std::max_element(g_counts, g_counts + 256);
	size_t lower_bound = total * 9 / 10 / 256;
	size_t upper_bound = total * 11 / 10 / 256;

	LOG_ALWAYS("min: %d max: %d", min, max);
	LOG_ALWAYS("lower: %d upper: %d", lower_bound, upper_bound);
	free(g_counts);

	ASSERT(lower_bound <= min);
	ASSERT(max <= upper_bound);
}

TEST_TEARDOWN(Random_ClearHandler)
{
	g_handler_fn = NULL;
}

extern void RLM3_Random_CB_ISR(uint32_t entropy)
{
	if (g_handler_fn)
		g_handler_fn(entropy);
}

