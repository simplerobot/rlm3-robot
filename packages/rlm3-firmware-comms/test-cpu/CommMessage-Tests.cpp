#include "Test.hpp"
#include "CommMessage.hpp"


TEST_CASE(MessageType_None)
{
	// Once in use, the id, size, and layout for a message must not be changed.  If this test
	// is failing, you probably modified an existing message instead of creating a new one.
	ASSERT(MESSAGE_TYPE_NONE == 0);
	ASSERT(sizeof(MESSAGE_BODY_NONE) == 2);
	ASSERT(offsetof(MESSAGE_BODY_NONE, type) == 0);
	ASSERT(offsetof(MESSAGE_BODY_NONE, crc) == 1);
}

TEST_CASE(MessageType_Version)
{
	// Once in use, the id, size, and layout for a message must not be changed.  If this test
	// is failing, you probably modified an existing message instead of creating a new one.
	ASSERT(MESSAGE_TYPE_VERSION == 1);
	ASSERT(sizeof(MESSAGE_BODY_VERSION) == 10);
	ASSERT(offsetof(MESSAGE_BODY_VERSION, type) == 0);
	ASSERT(offsetof(MESSAGE_BODY_VERSION, id) == 1);
	ASSERT(offsetof(MESSAGE_BODY_VERSION, version) == 5);
	ASSERT(offsetof(MESSAGE_BODY_VERSION, crc) == 9);
}

TEST_CASE(MessageType_Sync)
{
	// Once in use, the id, size, and layout for a message must not be changed.  If this test
	// is failing, you probably modified an existing message instead of creating a new one.
	ASSERT(MESSAGE_TYPE_SYNC == 2);
	ASSERT(sizeof(MESSAGE_BODY_SYNC) == 6);
	ASSERT(offsetof(MESSAGE_BODY_SYNC, type) == 0);
	ASSERT(offsetof(MESSAGE_BODY_SYNC, time) == 1);
	ASSERT(offsetof(MESSAGE_BODY_SYNC, crc) == 5);
}

TEST_CASE(MessageType_Control)
{
	// Once in use, the id, size, and layout for a message must not be changed.  If this test
	// is failing, you probably modified an existing message instead of creating a new one.
	ASSERT(MESSAGE_TYPE_CONTROL == 3);
	ASSERT(sizeof(MESSAGE_BODY_CONTROL) == 9);
	ASSERT(offsetof(MESSAGE_BODY_CONTROL, type) == 0);
	ASSERT(offsetof(MESSAGE_BODY_CONTROL, time) == 1);
	ASSERT(offsetof(MESSAGE_BODY_CONTROL, left) == 5);
	ASSERT(offsetof(MESSAGE_BODY_CONTROL, right) == 6);
	ASSERT(offsetof(MESSAGE_BODY_CONTROL, blade) == 7);
	ASSERT(offsetof(MESSAGE_BODY_CONTROL, crc) == 8);
}
