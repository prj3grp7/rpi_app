#pragma once
#pragma once
#include "Message.h"
#include "UserInfo.h"

class WebPageResponse : public Message
{
public:
	WebPageResponse(UserInfo & team1, UserInfo & team2)
	{
		team1_ = team1;
		team2_ = team2;
	}
	virtual ~WebPageResponse() {}
	UserInfo team1_;
	UserInfo team2_;
};
