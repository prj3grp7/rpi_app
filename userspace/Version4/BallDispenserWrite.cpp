#include "BallDispenserWrite.h"


BallDispenserWrite::BallDispenserWrite(const char * filename)
{
  filePointer_ = open(filename, O_RDWR);
	if (filePointer_ == -1) {
		cout << "BD_Write_OpenFile: Failed to open with err: " << strerror(errno) << endl << endl;
	}
}

void BallDispenserWrite::run()
{
  while (true)
  {
    unsigned long id;
    Message * msg = BallDispenserMsgQueue_.receive(id);
    dispatchGameControllerRequests(id, msg);
    delete msg;
  }

}

int BallDispenserWrite::writeBallDispenser(const char * command, int length)
{
  	cout << "BallDispenserWrite: ";
  writeI2C(command, length);
}

void BallDispenserWrite::dispatchGameControllerRequests(unsigned long id, Message * msg)
{
  switch(id)
  {
    case BallDispenser::ON : {
      cout << "BD_WRITE: DISPENSER ON" << endl;
      char sendBuffer[2] = {DispenseMessage.ON};
      writeBallDispenser(sendBuffer, 1);
    }
    break;

    case BallDispenser::OFF : {
      cout << "BD_WRITE: DISPENSER OFF" << endl;
      char sendBuffer[2] = {DispenseMessage.OFF};
      writeBallDispenser(sendBuffer, 1);
    }
    break;

    default :std::cout << "Default error: BallDispenserWrite Dispatcher" << std::endl;
  }
}
