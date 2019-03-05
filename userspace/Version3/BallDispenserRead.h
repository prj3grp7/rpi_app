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
#include "BallDispenserResponse.h"
#include "I2C.h"

class BallDispenserRead : public ThreadFunctor, public I2C
{
public:
  BallDispenserRead(const char * filename, MsgQueue * GameControllerQueue);
  virtual void translateMessage();
  int readBallDispenser();
  const char * getBallDispenserReadBuffer();
  virtual void run();

private:
  MsgQueue * GameControllerMsgQueue_;
};
