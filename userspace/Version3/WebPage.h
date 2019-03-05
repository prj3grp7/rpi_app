#pragma once
#include "UserInfo.h"
#include "Message.h"
#include "MessageQueue.h"
#include "Thread.h"
#include "ThreadFunctor.h"
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <string_view>
#include <uWS/uWS.h>
#include <sstream>


class WebPage : public ThreadFunctor
{
public:
	WebPage(MsgQueue * GameControllerMsgQueue_);
	~WebPage() {}
	virtual void run();
	void WSInit();

	uWS::Hub h;
private:
	UserInfo team1_;
	UserInfo team2_;
	MsgQueue WebSocketMsgQueue_;
	MsgQueue * GameControllerMsgQueue_;

};
