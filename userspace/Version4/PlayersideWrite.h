#pragma once
#include <iostream>
#include "MessageQueue.h"
#include "Message.h"
#include "ThreadFunctor.h"
#include "PlayersideResponse.h"
#include "GameControllerRequest.h"
#include "MsgProtocol.h"
#include "I2C.h"
#include <cstdint>
#include "TeamColorsMsg.h"

using namespace std;

class PlayersideWrite : public ThreadFunctor, public I2C
{
public:
	PlayersideWrite(const char * filename, int playerside);

	//Thread control for Playerside
	virtual void run();

	//i2c-protocol for writing data
	int writePlayerside(const char * command, int length);

	//Dispatcher
	void dispatchGameControllerRequests(unsigned long id, Message * msg);

	MsgQueue PlayersideMsgQueue_;

private:
	int playerside_;

};
