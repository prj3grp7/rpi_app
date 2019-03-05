#pragma once
#include <iostream>
#include "MessageQueue.h"
#include "Message.h"
#include "ThreadFunctor.h"
#include "BallDispenserResponse.h"
#include "GameControllerRequest.h"
#include "MsgProtocol.h"
#include "I2C.h"

using namespace std;

class BallDispenserWrite : public ThreadFunctor, public I2C
{
public:
  BallDispenserWrite(const char * filename);

  virtual void run();

  int writeBallDispenser(const char * command, int length);

  void dispatchGameControllerRequests(unsigned long id, Message * msg);

  virtual ~BallDispenserWrite() {}

  MsgQueue BallDispenserMsgQueue_;
};
