#pragma once
#include <iostream>
#include "MessageQueue.h"
#include "Message.h"
#include "ThreadFunctor.h"
#include "Thread.h"
#include "PlayersideResponse.h"
#include "GameControllerRequest.h"
#include "MsgProtocol.h"
#include <bitset>
#include "I2C.h"

using namespace std;

class PlayersideRead : public ThreadFunctor, public I2C
{
public:
	PlayersideRead(const char * filename, MsgQueue * GameControllerQueue, int playerside);

	//Translate message handler
	virtual void translateMessage();

	//i2c-protocol for reading data
	int readPlayerside(); //Read from file via filedescripter
	const char * getPlayersideReadBuffer();

	//Thread control for Playerside
	virtual void run();

	virtual ~PlayersideRead() {}

private:
	MsgQueue * GameControllerQueue_;
	int playerside_;
};
