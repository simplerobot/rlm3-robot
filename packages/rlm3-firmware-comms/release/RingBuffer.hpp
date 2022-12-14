#pragma once

#include "rlm3-base.h"
#include "rlm3-task.h"


template <size_t SIZE>
class RingBuffer
{
	static_assert(SIZE > 0, "RingBuffer cannot be empty.");
	static_assert(!(SIZE & (SIZE - 1)), "RingBuffer currently only supports power of two sizes.");

public:
	RingBuffer() = default;

	void put(uint8_t data)
	{
		// Do a single read to the volatile variable for our end of the buffer.
		size_t head = m_head;
		// Wait for there to be room in the buffer.
		while (is_full(head, m_tail))
			RLM3_Take();
		// Add the data to the buffer.
		m_buffer[head % SIZE] = data;
		// Notify the isr reader there is data in the buffer.
		m_head = head + 1;
	}

	bool put_with_timeout(uint8_t data, RLM3_Time timeout_ms) __attribute__ ((warn_unused_result))
	{
		return put_until(data, RLM3_GetCurrentTime(), timeout_ms);
	}

	bool put_until(uint8_t data, RLM3_Time start_time, RLM3_Time delay_ms) __attribute__ ((warn_unused_result))
	{
		// Do a single read to the volatile variable for our end of the buffer.
		size_t head = m_head;
		// Wait for there to be room in the buffer.
		while (is_full(head, m_tail))
			if (!RLM3_TakeUntil(start_time, delay_ms))
				return false;
		// Add the data to the buffer.
		m_buffer[head % SIZE] = data;
		// Notify the reader there is data in the buffer.
		m_head = head + 1;
		return true;
	}

	bool put_isr(uint8_t data, RLM3_Task task) __attribute__ ((warn_unused_result))
	{
		// Do a single read of the volatile variables.
		size_t head = m_head;
		size_t tail = m_tail;
		// If the buffer is full, fail fast.
		if (is_full(head, tail))
			return false;
		// Add the data to the buffer.
		m_buffer[head % SIZE] = data;
		// Notify the reader there is data in the buffer.
		m_head = head + 1;
		if (is_empty(head, tail))
			RLM3_GiveFromISR(task);
		return true;
	}

	void reset_isr(RLM3_Task task)
	{
		// The reset takes space so the queue will always be non-empty.
		size_t head = m_head;
		m_reset = head + 1;
		m_has_reset = true;
		m_head = head + 1;
		if (is_empty(head, m_tail))
			RLM3_GiveFromISR(task);
	}

	bool get(uint8_t& data)
	{
		// Do a single read to the volatile variable for our end of the buffer.
		size_t tail = m_tail;
		// Wait until there is data in the buffer.
		while (is_empty(m_head, tail))
			RLM3_Take();
		// Don't return any data if the buffer has a pending reset.
		if (m_has_reset)
			return false;
		// Get the data from the buffer;
		data = m_buffer[tail % SIZE];
		// Free up this space in the buffer.
		m_tail = tail + 1;
		return true;
	}

	bool get_with_timeout(uint8_t& data, RLM3_Time timeout_ms) __attribute__ ((warn_unused_result))
	{
		return get_until(data, RLM3_GetCurrentTime(), timeout_ms);
	}

	bool get_until(uint8_t& data, RLM3_Time start_time, RLM3_Time delay_ms) __attribute__ ((warn_unused_result))
	{
		// Do a single read to the volatile variable for our end of the buffer.
		size_t tail = m_tail;
		// Wait until there is data in the buffer.
		while (is_empty(m_head, tail))
			if (!RLM3_TakeUntil(start_time, delay_ms))
				return false;
		// Don't return any data if the buffer has a pending reset.
		if (m_has_reset)
			return false;
		// Get the data from the buffer;
		data = m_buffer[tail % SIZE];
		// Free up this space in the buffer.
		m_tail = tail + 1;
		return true;
	}

	bool get_isr(uint8_t& data, RLM3_Task task) __attribute__ ((warn_unused_result))
	{
		// Do a single read of the volatile variables.
		size_t head = m_head;
		size_t tail = m_tail;
		// If the buffer is empty, fail fast.
		if (is_empty(head, tail))
			return false;
		// Don't return any data if the buffer has a pending reset.
		if (m_has_reset)
			return false;
		// Get data from the buffer;
		data = m_buffer[tail % SIZE];
		// Notify the writer that there is more space in the buffer.
		m_tail = tail + 1;
		if (is_full(head, tail))
			RLM3_GiveFromISR(task);
		return true;
	}

	bool is_reset() const
	{
		return m_has_reset;
	}

	void clear_reset()
	{
		m_has_reset = false;
		m_tail = m_reset;
	}

private:
	static bool is_empty(size_t head, size_t tail) __attribute__ ((warn_unused_result))
	{
		return (head == tail);
	}

	static bool is_full(size_t head, size_t tail) __attribute__ ((warn_unused_result))
	{
		return (head - tail >= SIZE);
	}

private:
	volatile size_t m_head = 0;
	volatile size_t m_tail = 0;
	volatile size_t m_reset = 0;
	volatile bool m_has_reset = false;
	volatile uint8_t m_buffer[SIZE];
};
