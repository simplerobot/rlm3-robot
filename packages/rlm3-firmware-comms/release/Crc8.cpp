#include "Crc8.hpp"

/*
 * Code was adapted from pycrc-0.10.0 output
 *
 * python3 src/pycrc.py --model=crc-8 --generate c -o out-crc-8-tdd-2.c --algorithm=table-driven --table-idx-width=2
 */

static const uint8_t CRC_TABLE[16] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d
};

void Crc8::reset()
{
	m_state = 0;
}

void Crc8::add(uint8_t data)
{
	m_state = CRC_TABLE[((m_state >> 4) ^ (data >> 4)) & 0x0F] ^ (m_state << 4);
	m_state = CRC_TABLE[((m_state >> 4) ^ (data >> 0)) & 0x0F] ^ (m_state << 4);
}

uint8_t Crc8::get() const
{
	return m_state;
}
