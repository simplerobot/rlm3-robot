#pragma once

#include "rlm3-base.h"


class Crc8
{
public:
	Crc8() = default;

	void reset();
	void add(uint8_t data);
	uint8_t get() const;

	void add(const void* data, size_t size);

private:
	uint8_t m_state = 0;
};
