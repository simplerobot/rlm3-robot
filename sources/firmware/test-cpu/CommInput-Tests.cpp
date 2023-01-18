#include "../../firmware/main/CommInput.hpp"
#include "Test.hpp"
#include "rlm3-sim.hpp"
#include "Crc8.hpp"


static size_t g_control_call_count = 0;
static uint32_t g_control_time = 0;
static float g_control_left = 0;
static float g_control_right = 0;
static float g_control_blade = 0;

extern void CommInput_Control_Callback(uint32_t time, float left, float right, float blade)
{
	g_control_call_count++;
	g_control_time = time;
	g_control_left = left;
	g_control_right = right;
	g_control_blade = blade;
}

static void AddMessageInterrupts(Crc8& crc, std::initializer_list<uint8_t> bytes, bool expect_success = true)
{
	for (uint8_t b : bytes)
		SIM_AddInterrupt([b]{ ASSERT(CommInput_PutMessageByteISR(b)); });
	for (uint8_t b : bytes)
		crc.add(b);
	uint8_t crc_sum = crc.get();
	SIM_AddInterrupt([crc_sum, expect_success]{ ASSERT(CommInput_PutMessageByteISR(crc_sum) == expect_success); });
}

TEST_CASE(CommInput_Abort)
{
	SIM_AddInterrupt([] { CommInput_AbortTaskISR(); });

	CommInput_RunTask();
}

TEST_CASE(CommInput_AbortRestart)
{
	SIM_AddInterrupt([] { CommInput_AbortTaskISR(); });
	SIM_AddInterrupt([] { CommInput_AbortTaskISR(); });

	CommInput_RunTask();
	CommInput_RunTask();
}

TEST_CASE(CommInput_HappyCase)
{
	Crc8 crc;
	SIM_AddInterrupt([]{ CommInput_ResetPipeISR(); });
	AddMessageInterrupts(crc, {
			4, // MESSAGE_TYPE_CONTROL
			0x12, 0x34, 0x56, 0x78, // time
			121, // left
			(uint8_t)-7, // right
			13, // blade
	});
	// Tell the input thread to stop.
	SIM_AddInterrupt([] { CommInput_AbortTaskISR(); });
	g_control_call_count = 0;

	CommInput_RunTask();

	ASSERT(g_control_call_count == 1);
	ASSERT(g_control_time == 0x12345678);
	ASSERT(std::abs(g_control_left - 0.95) < 0.01);
	ASSERT(std::abs(g_control_right - -0.05) < 0.01);
	ASSERT(std::abs(g_control_blade - 0.10) < 0.01);
}

TEST_CASE(CommInput_MultipleMessages)
{
	Crc8 crc;
	SIM_AddInterrupt([]{ CommInput_ResetPipeISR(); });
	AddMessageInterrupts(crc, {
			4, // MESSAGE_TYPE_CONTROL
			0x12, 0x34, 0x56, 0x78, // time
			122, // left
			(uint8_t)-7, // right
			5, // blade
	});
	AddMessageInterrupts(crc, {
			1, // MESSAGE_TYPE_NONE
	});
	AddMessageInterrupts(crc, {
			2, // MESSAGE_TYPE_VERSION
			0x9A, 0xBC, 0xDE, 0xF0, // id
			0x11, 0x22, 0x33, 0x44, // version
	});
	AddMessageInterrupts(crc, {
			3, // MESSAGE_TYPE_SYNC
			0x13, 0x9B, 0x13, 0x9B, // time
	});
	// Tell the input thread to stop.
	SIM_AddInterrupt([] { CommInput_AbortTaskISR(); });

	CommInput_RunTask();
}

TEST_CASE(CommInput_BadCrc)
{
	Crc8 crc;
	crc.add(0x12); // Mess up the crc calculation.
	SIM_AddInterrupt([]{ CommInput_ResetPipeISR(); });
	AddMessageInterrupts(crc, {
			4, // MESSAGE_TYPE_CONTROL
			0x12, 0x34, 0x56, 0x78, // time
			122, // left
			(uint8_t)-7, // right
			5, // blade
	}, false);
	// Tell the input thread to stop.
	SIM_AddInterrupt([] { CommInput_AbortTaskISR(); });
	g_control_call_count = 0;

	CommInput_RunTask();

	ASSERT(g_control_call_count == 0);
}

TEST_CASE(CommInput_ResetPipe)
{
	Crc8 crc1, crc2;
	SIM_AddInterrupt([]{ CommInput_ResetPipeISR(); });
	AddMessageInterrupts(crc1, {
			4, // MESSAGE_TYPE_CONTROL
			0x12, 0x34, 0x56, 0x78, // time
			122, // left
			(uint8_t)-7, // right
			5, // blade
	});
	AddMessageInterrupts(crc1, {
			1, // MESSAGE_TYPE_NONE
	});
	SIM_AddInterrupt([]{ CommInput_ResetPipeISR(); });
	AddMessageInterrupts(crc2, {
			2, // MESSAGE_TYPE_VERSION
			0x9A, 0xBC, 0xDE, 0xF0, // id
			0x11, 0x22, 0x33, 0x44, // version
	});
	AddMessageInterrupts(crc2, {
			3, // MESSAGE_TYPE_SYNC
			0x13, 0x9B, 0x13, 0x9B, // time
	});
	// Tell the input thread to stop.
	SIM_AddInterrupt([] { CommInput_AbortTaskISR(); });
	g_control_call_count = 0;

	CommInput_RunTask();
}

TEST_CASE(CommInput_ResetPipeMidway)
{
	Crc8 crc1, crc2;
	SIM_AddInterrupt([]{ CommInput_ResetPipeISR(); });
	AddMessageInterrupts(crc1, {
			4, // MESSAGE_TYPE_CONTROL
			0x12, 0x34, 0x56, 0x78, // time
			122, // left
			(uint8_t)-7, // right
			5, // blade
	});
	AddMessageInterrupts(crc1, {
			1, // MESSAGE_TYPE_NONE
	});
	SIM_AddInterrupt([]{ CommInput_PutMessageByteISR(3); }); // Start a new message.
	SIM_AddInterrupt([]{ CommInput_ResetPipeISR(); });
	AddMessageInterrupts(crc2, {
			2, // MESSAGE_TYPE_VERSION
			0x9A, 0xBC, 0xDE, 0xF0, // id
			0x11, 0x22, 0x33, 0x44, // version
	});
	AddMessageInterrupts(crc2, {
			3, // MESSAGE_TYPE_SYNC
			0x13, 0x9B, 0x13, 0x9B, // time
	});
	// Tell the input thread to stop.
	SIM_AddInterrupt([] { CommInput_AbortTaskISR(); });
	g_control_call_count = 0;

	CommInput_RunTask();
}

TEST_CASE(CommInput_InvalidMessageType)
{
	SIM_AddInterrupt([]{ CommInput_ResetPipeISR(); });
	SIM_AddInterrupt([] { ASSERT(!CommInput_PutMessageByteISR(0x67)); }); // 0x67 = Invalid message type.
	SIM_AddInterrupt([] { CommInput_AbortTaskISR(); });

	CommInput_RunTask();
}

TEST_CASE(CommInput_OverflowBuffer)
{
	SIM_AddInterrupt([]{ CommInput_ResetPipeISR(); });
	size_t message_count = 0;
	SIM_AddInterrupt([&message_count] {
		Crc8 crc;
		std::vector<uint8_t> general_message = {
				4, // MESSAGE_TYPE_CONTROL
				0x12, 0x34, 0x56, 0x78, // time
				122, // left
				(uint8_t)-7, // right
				5, // blade
		};
		// Add messages until an overflow occurs.
		while (message_count < 1024)
		{
			for (uint8_t b : general_message)
			{
				crc.add(b);
				if (!CommInput_PutMessageByteISR(b))
					return;
			}
			if (!CommInput_PutMessageByteISR(crc.get()))
				return;
			message_count++;
		}
		ASSERT(false); // We added too many messages.
	});
	// Make sure we can recover and read another message.
	SIM_AddInterrupt([]{ CommInput_ResetPipeISR(); });
	Crc8 crc2;
	AddMessageInterrupts(crc2, {
			4, // MESSAGE_TYPE_CONTROL
			0x12, 0x34, 0x56, 0x78, // time
			122, // left
			(uint8_t)-7, // right
			5, // blade
	});
	SIM_AddInterrupt([] { CommInput_AbortTaskISR(); });
	g_control_call_count = 0;

	CommInput_RunTask();

	ASSERT(g_control_call_count == message_count + 1);
}


