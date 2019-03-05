#pragma once
#include "UserInfo.h"
#include "Message.h"
#include "MessageQueue.h"
#include "Thread.h"
#include "ThreadFunctor.h"
#include "DisplayRequest.h"
#include <iostream>

class Display : public ThreadFunctor
{
public:
  Display(UserInfo & team1, UserInfo & team2) {
    team1_ = team1
    team2_ = team2
  }
  ~Display() {}

  //ThreadFunction
  virtual void run();

  //Dispatcher
  void dispatcherDisplayMessage(unsigned long id, Message * msg);

  //Handlers
  void handleDisplayShowIdle();
  void handleDisplayShowPlaceCups();
  void handleDisplayShowInfo();
  void handleDisplayPlaying();
  void handleUpdateCups();
  void handleUpdateWin(DisplayRequest * msg);

private:
  const UserInfo * team1_;
  const UserInfo * team2_;
  MsgQueue DisplayMsgQueue_;
}
