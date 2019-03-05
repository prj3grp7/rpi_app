#pragma once
#include "Message.h"

class EndGameCfm : public Message
{
public:
	EndGameCfm(int playerside) : playerside_(playerside)
	{
	}
	int playerside_;
};
