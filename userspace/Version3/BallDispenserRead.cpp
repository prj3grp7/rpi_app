#include "BallDispenserRead.h"

using namespace std;

BallDispenserRead::BallDispenserRead(const char * filename, MsgQueue * GameControllerMsgQueue)
  :  GameControllerMsgQueue_(GameControllerMsgQueue)
{
  filePointer_ = open(filename, O_RDWR);
	if (filePointer_ == -1) {
		cout << "Failed to open with err: " << strerror(errno) << endl << endl;
	}
}

void BallDispenserRead::translateMessage()
{
  BallDispenserResponse * msg = new BallDispenserResponse(readBuffer_);
  GameControllerMsgQueue_->send(BallDispenser::NEW_INFO, msg);
}

int BallDispenserRead::readBallDispenser()
{
  cout << "PlayersideRead" << endl;
  readI2C();
}

const char * BallDispenserRead::getBallDispenserReadBuffer()
{
  return getReadBuffer();
}

void BallDispenserRead::run()
{
  while(true)
  {
    /*
    int num_byte = readBallDispenser();
    for (int i = 0; i < num_byte; i++) {
      cout << bitset<8>(I2C::readBuffer_[i]) << endl;
    }


    //If something has been received
    if (num_byte > 0)
    {
      translateMessage();
    }
    */

  }
}
