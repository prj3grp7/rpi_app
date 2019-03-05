#pragma once
#include "Message.h"

class DisplayRequest : public Message
{
public:
  DisplayRequest(const int playerside) : playerside_(playerside)
  {

  }
  const int playerside_;
}
