#pragma once

#include "rlm3-base.h"


enum MessageType : uint8_t
{
	MESSAGE_TYPE_INVALID	= 0,
	MESSAGE_TYPE_NONE		= 1,
	MESSAGE_TYPE_VERSION	= 2,
	MESSAGE_TYPE_SYNC		= 3,
	MESSAGE_TYPE_CONTROL	= 4,
};

extern const char* ToString(MessageType type);
extern size_t GetMessageSize(MessageType type);

struct __attribute__((__packed__)) MESSAGE_BODY_NONE
{
	MessageType	type;
	uint8_t		crc;
};

struct __attribute__((__packed__)) MESSAGE_BODY_VERSION
{
	MessageType	type;
	uint32_t	id;
	uint32_t	version;
	uint8_t		crc;
};

struct __attribute__((__packed__)) MESSAGE_BODY_SYNC
{
	MessageType	type;
	uint32_t	time;
	uint8_t		crc;
};

struct __attribute__((__packed__)) MESSAGE_BODY_CONTROL
{
	MessageType	type;
	uint32_t	time;
	int8_t		left;
	int8_t		right;
	int8_t		blade;
	uint8_t		crc;
};
