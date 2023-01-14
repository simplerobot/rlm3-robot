#include "Test.hpp"
#include "Crc8.hpp"
#include <cstdio>
#include <initializer_list>


TEST_CASE(Crc8_Constructor)
{
	Crc8 test;

	ASSERT(test.get() == 0);
}

static uint8_t CalculateCrc(const char* s)
{
	Crc8 test;
	for (size_t i = 0; s[i] != 0; i++)
		test.add(s[i]);
	return test.get();
}

static uint8_t CalculateCrc(const std::initializer_list<uint8_t>& v)
{
	Crc8 test;
	for (uint8_t x : v)
		test.add(x);
	return test.get();
}

TEST_CASE(Crc8_ValidateTestVectors)
{
	ASSERT(0x00 == CalculateCrc(""));
	ASSERT(0x25 == CalculateCrc("Hello World"));
	ASSERT(0x00 == CalculateCrc({ 0x00, 0x00, 0x00, 0x00 }));
	ASSERT(0x2F == CalculateCrc({ 0xF2, 0x01, 0x83 }));
	ASSERT(0xB1 == CalculateCrc({ 0x0F, 0xAA, 0x00, 0x55 }));
	ASSERT(0x11 == CalculateCrc({ 0x00, 0xFF, 0x55, 0x11 }));
	ASSERT(0x59 == CalculateCrc({ 0x33, 0x22, 0x55, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }));
	ASSERT(0xB1 == CalculateCrc({ 0x92, 0x6B, 0x55 }));
	ASSERT(0xDE == CalculateCrc({ 0xFF, 0xFF, 0xFF, 0xFF }));
	ASSERT(0x1a == CalculateCrc({ 0xBE, 0xEF }));
}
