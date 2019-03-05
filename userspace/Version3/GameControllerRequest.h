#pragma once
#include "Message.h"

class GameControllerRequest : public Message
{
public:
	GameControllerRequest(char * writePtr) : writePtr_(writePtr)
	{

	}
	char * writePtr_ = nullptr;
};