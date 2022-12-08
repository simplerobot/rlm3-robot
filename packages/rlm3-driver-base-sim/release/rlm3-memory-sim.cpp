#include "rlm3-memory.h"
#include "Test.hpp"
#include "logger.h"
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <stdlib.h>
#include <stdexcept>


LOGGER_ZONE(SIM_MEMORY);


static bool g_is_initialized = false;
static bool g_was_used = false;

static uint8_t* g_memory = NULL; // A buffer allocated 3 times larger than needed.  The outside pages are marked as unreachable and the middle we control separately.


extern void RLM3_MEMORY_Init()
{
	ASSERT(!g_is_initialized);
	ASSERT(g_memory != NULL);
	g_is_initialized = true;
	g_was_used = true;
	int mprotect_result = ::mprotect(g_memory + RLM3_EXTERNAL_MEMORY_SIZE, RLM3_EXTERNAL_MEMORY_SIZE, PROT_READ | PROT_WRITE);
	ASSERT(mprotect_result == 0);
}

extern void RLM3_MEMORY_Deinit()
{
	ASSERT(g_is_initialized);
	g_is_initialized = false;
	::mprotect(g_memory + RLM3_EXTERNAL_MEMORY_SIZE, RLM3_EXTERNAL_MEMORY_SIZE, PROT_NONE);
}

extern bool RLM3_MEMORY_IsInit()
{
	return g_is_initialized;
}

extern uint8_t* SIM_MEMORY_GetBaseAddress()
{
	ASSERT(g_is_initialized);
	return g_memory + RLM3_EXTERNAL_MEMORY_SIZE;
}

extern "C" const char *__asan_default_options()
{
	return "handle_segv=0";
}

static void signal_handler(int sig, siginfo_t *info, void *ucontext)
{
	uint8_t* addr = (uint8_t*) info->si_addr;

	if (g_memory != NULL && g_memory <= addr&& addr < g_memory + 3 * RLM3_EXTERNAL_MEMORY_SIZE)
	{
		ssize_t offset = (addr - g_memory) - RLM3_EXTERNAL_MEMORY_SIZE;
		if (offset < 0)
			std::printf("Illegal memory access %zd bytes before the external memory\n", -offset);
		else if (offset >= RLM3_EXTERNAL_MEMORY_SIZE)
			std::printf("Illegal memory access %zd bytes after the external memory\n", offset - RLM3_EXTERNAL_MEMORY_SIZE);
		else if (!g_is_initialized)
			std::printf("External memory was accessed at offset 0x%zx (%zd) while external memory was not initialized.\n", offset, offset);
		else
			std::printf("Internal error.  External memory access should have worked\n");
	}
	else
		std::printf("Got SIGSEGV at address: %p  External Mem: %p\n", addr, g_memory);
	std::fflush(stdout);

	FAIL("Illegal Memory Access");
}

TEST_START(MEMORY_SETUP)
{
	if (g_memory == NULL)
	{
		struct sigaction action = {};
		action.sa_sigaction = signal_handler;
		action.sa_flags = SA_SIGINFO | SA_NODEFER;
		int result = ::sigaction(SIGSEGV, &action, nullptr);
		if (result != 0)
			std::printf("sigaction failed: errno: %d (%s)\n", errno, strerror(errno));
		ASSERT(result == 0);

		long int page_size = ::sysconf(_SC_PAGE_SIZE);
		ASSERT(RLM3_EXTERNAL_MEMORY_SIZE % page_size == 0);
		void* memory = ::mmap(nullptr, 3 * RLM3_EXTERNAL_MEMORY_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (memory == MAP_FAILED)
			std::printf("Memory: %p  errno: %d (%s)\n", memory, errno, strerror(errno));
		ASSERT(memory != MAP_FAILED);
		g_memory = (uint8_t*)memory;
	}
	if (g_was_used)
	{
		if (!g_is_initialized)
			RLM3_MEMORY_Init();
		std::memset(SIM_MEMORY_GetBaseAddress(), 0, RLM3_EXTERNAL_MEMORY_SIZE);

	}
	if (g_is_initialized)
		RLM3_MEMORY_Deinit();
}


