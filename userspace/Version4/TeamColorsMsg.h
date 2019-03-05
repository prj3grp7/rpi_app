#pragma once
#include "Message.h"
#include <cstdint>


class TeamColorsMsg : public Message
{
public:
  TeamColorsMsg(uint8_t team1red, uint8_t team1green, uint8_t team1blue, uint8_t team2red, uint8_t team2green, uint8_t team2blue)
    : team1red_(team1red), team1green_(team1green), team1blue_(team1blue), team2red_(team2red), team2green_(team2green), team2blue_(team2blue)
  {

  }

uint8_t team1red_;
uint8_t team1green_;
uint8_t team1blue_;

uint8_t team2red_;
uint8_t team2green_;
uint8_t team2blue_;
};
