#pragma once
#pragma once
#include "Message.h"
#include "UserInfo.h"

class WebPageResponse : public Message
{
public:
	WebPageResponse(const UserInfo & team1, const UserInfo & team2)
	{
		//if flag != 0
		team1_ = team1;
		team2_ = team2;
	}
	virtual ~WebPageResponse() {}
	UserInfo team1_;
	UserInfo team2_;
};
