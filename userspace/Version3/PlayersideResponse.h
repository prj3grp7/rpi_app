#pragma once
#include "Message.h"

class PlayersideResponse : public Message
{
public:
	PlayersideResponse(const char * readPtr = nullptr, int playerside = 0) : readPtr_(readPtr),
		playerside_(playerside)
	{

	}
	const char * readPtr_ = nullptr;
	const int playerside_ = 0;
};
