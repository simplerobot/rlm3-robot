#include "CommInput.hpp"
#include "rlm3-task.h"
#include "CommMessage.hpp"
#include "Crc8.hpp"
#include "logger.h"

LOGGER_ZONE(COMM_IN);


static constexpr size_t BUFFER_SIZE = 64;
static volatile size_t g_head = 0;
static volatile size_t g_tail = 0;
static volatile size_t g_next = 0;
static volatile uint8_t g_buffer[BUFFER_SIZE];

static size_t g_current_message_size_remaining = 0;

static RLM3_Task g_task = nullptr;
static volatile bool g_abort = false;

static Crc8 g_crc;


static uint8_t ExtractU8(size_t offset)
{
	return g_buffer[offset % BUFFER_SIZE];
}

static uint32_t ExtractU32(size_t offset)
{
	uint32_t result = 0;
	for (size_t i = 0; i < sizeof(result); i++)
		result = ((result << 8) | ExtractU8(offset + i));
	return result;
}

static int8_t ExtractI8(size_t offset)
{
	return (int8_t)ExtractU8(offset);
}

extern void CommInput_RunTask()
{
	g_task = RLM3_GetCurrentTask();
	while (!g_abort)
	{
		// Do a single read of the volatile variables.
		size_t tail = g_tail;
		size_t head = g_head;

		// Check if a message is available.
		if (head != tail)
		{
			// Get the length of this message.
			MessageType message_type = (MessageType)ExtractU8(tail);
			size_t message_size = GetMessageSize(message_type);
			if (message_size == 0)
			{
				// This should not happen since we sent completed messages.  Clear the buffer to sync back up.
				LOG_ERROR("Internal Error: Invalid message type %d", message_type);
				g_tail = head;
				break;
			}
			if (head - tail < message_size)
			{
				// This should not happen since we sent completed messages.  Clear the buffer to sync back up.
				LOG_ERROR("Internal Error: Message not in buffer %zd %zd", head - tail, message_size);
				g_tail = head;
				break;
			}
			// TODO: process one message.
			switch (message_type)
			{
			case MESSAGE_TYPE_NONE:
				break;
			case MESSAGE_TYPE_CONTROL:
				{
					uint32_t time = ExtractU32(tail + offsetof(MESSAGE_BODY_CONTROL, time));
					int8_t left = ExtractI8(tail + offsetof(MESSAGE_BODY_CONTROL, left));
					int8_t right = ExtractI8(tail + offsetof(MESSAGE_BODY_CONTROL, right));
					int8_t blade = ExtractI8(tail + offsetof(MESSAGE_BODY_CONTROL, blade));
					CommInput_Control_Callback(time, left, right, blade);
				}
				break;
			case MESSAGE_TYPE_VERSION:
			case MESSAGE_TYPE_SYNC:
				LOG_WARN("Unexpected %s", ToString(message_type));
				break;
			default:
				LOG_ERROR("Internal Error: Invalid message type %d", message_type);
				message_size = head - tail; // Skip everything else in the buffer.
				break;
			}

			// Move past this message.
			g_tail = tail + message_size;
		}
		else
		{
			// Wait until a message arrives (or until we abort).
			RLM3_Take();
		}
	}
	g_abort = false;
	g_task = nullptr;
}

extern void CommInput_AbortTask()
{
	g_abort = true;
	RLM3_Give(g_task);
}

extern void CommInput_AbortTaskISR()
{
	g_abort = true;
	RLM3_GiveFromISR(g_task);
}

extern bool CommInput_PutMessageByteISR(uint8_t byte)
{
	// Do a single read of volatile variables.
	size_t next = g_next;
	size_t tail = g_tail;
	size_t head = g_head;

	// Fail if the buffer is full.
	if (next - tail >= BUFFER_SIZE)
	{
		LOG_ERROR("Overflow");
		return false;
	}

	// Add the data temporarily to the buffer.
	g_buffer[next % BUFFER_SIZE] = byte;
	g_next = next + 1;

	// If we are not reading one, start a new message.
	if (g_current_message_size_remaining == 0)
	{
		g_current_message_size_remaining = GetMessageSize((MessageType)byte);
		if (g_current_message_size_remaining == 0)
		{
			LOG_ERROR("Unknown Message %d", byte);
			return false;
		}
	}

	// If this is the last byte of a message, publish it to the task.
	if (--g_current_message_size_remaining == 0)
	{
		if (byte != g_crc.get())
		{
			LOG_ERROR("Invalid checksum %d %d", byte, g_crc.get());
			return false;
		}
		// Do a single read of the volatile variables.
		// Publish the temporary data.
		g_head = next + 1;
		// Notify the reader task there is now data in the buffer.
		if (head == tail)
			RLM3_GiveFromISR(g_task);
	}
	else
	{
		g_crc.add(byte);
	}

	return true;
}

extern void CommInput_ResetPipeISR()
{
	// Do a single read of the volatile variables.
	size_t head = g_head;
	// Rewind our buffer to the last published point.
	g_next = head;
	// Start our crc calculation.
	g_crc.reset();
	// Expect a new message.
	g_current_message_size_remaining = 0;
}

extern __attribute__((weak)) void CommInput_Control_Callback(uint32_t time, int8_t left, int8_t right, int8_t blade)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
}

