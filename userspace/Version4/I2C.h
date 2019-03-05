#pragma once

extern "C"
{
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
}
#include <iostream>

class I2C
{
public:
  I2C(){};
  virtual void translateMessage(){};
  int readI2C();
  const char * getReadBuffer();

  int writeI2C(const char * command, int length);

  virtual ~I2C(){}

  int filePointer_;

protected:
  char readBuffer_[8] = {0};
  char writeBuffer_[8] = {0};
};
