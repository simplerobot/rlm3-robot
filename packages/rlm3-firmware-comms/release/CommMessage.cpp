#include "CommMessage.hpp"


extern const char* ToString(MessageType type)
{
	switch (type)
	{
	case MESSAGE_TYPE_NONE:		return "NONE";
	case MESSAGE_TYPE_VERSION:	return "VERSION";
	case MESSAGE_TYPE_SYNC:		return "SYNC";
	case MESSAGE_TYPE_CONTROL:	return "CONTROL";
	default:					return "UNKNOWN";
	}
}

extern size_t GetMessageSize(MessageType type)
{
	switch (type)
	{
	case MESSAGE_TYPE_NONE:		return sizeof(MESSAGE_BODY_NONE);
	case MESSAGE_TYPE_VERSION:	return sizeof(MESSAGE_BODY_VERSION);
	case MESSAGE_TYPE_SYNC:		return sizeof(MESSAGE_BODY_SYNC);
	case MESSAGE_TYPE_CONTROL:	return sizeof(MESSAGE_BODY_CONTROL);
	default:					return 0;
	}
}

