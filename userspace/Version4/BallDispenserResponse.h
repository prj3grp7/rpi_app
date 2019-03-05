#pragma once
#include "Message.h"

class BallDispenserResponse : public Message
{
public:
	BallDispenserResponse(const char * readPtr) : readPtr_(readPtr)
	{

	}
	const char * readPtr_;
};
