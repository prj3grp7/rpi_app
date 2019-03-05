#include "PlayersideWrite.h"

PlayersideWrite::PlayersideWrite(const char * filename, int playerside)
	: playerside_(playerside)
{
	filePointer_ = open(filename, O_RDWR);
	if (filePointer_ == -1) {
		cout << "PS_WriteOpen: Failed to open with err: " << strerror(errno) << endl << endl;
	}
}

void PlayersideWrite::run()
{
	while (true)
	{
		unsigned long id;
		Message * msg = PlayersideMsgQueue_.receive(id);
		dispatchGameControllerRequests(id, msg);
		delete msg;
	}
}

int PlayersideWrite::writePlayerside(const char * command, int length)
{
	writeI2C(command, length);
}

void PlayersideWrite::dispatchGameControllerRequests(unsigned long id, Message * msg)
{
		switch(id)
		{
			case SYSTEM_STATE::IDLE :{
				cout << "PS_WRITE" << playerside_ << ": STATE IDLE" << endl;
				char sendBuffer[2] = {PlayersideMessage.IDLE};
				writePlayerside(sendBuffer, 1);
			}
			break;

			case SYSTEM_STATE::STARTING :
			{
				cout << "PS_WRITE" << playerside_ << ": STATE STARTING" << endl;
				char sendBuffer[2] = {PlayersideMessage.STARTING};
				writePlayerside(sendBuffer, 1);
				char sendBuffer1[5] = {0x21,0x01,0x01,0x01,0x01};
				writePlayerside(sendBuffer1, 5);
			}
			break;

			case SYSTEM_STATE::PLAYING :{
				cout << "PS_WRITE" << playerside_ << ": STATE PLAYING" << endl;
				char sendBuffer[2] = {PlayersideMessage.PLAYING};
				writePlayerside(sendBuffer, 1);
			}
			break;

			case SYSTEM_STATE::WON :{
				cout << "PS_WRITE" << playerside_ << ": STATE WON" << endl;
				char sendBuffer[2] = {PlayersideMessage.WON};
				writePlayerside(sendBuffer, 1);
			}
			break;

			case SYSTEM_STATE::LOST : {
				cout << "PS_WRITE" << playerside_ << ": STATE LOST" << endl;
				char sendBuffer[2] = {PlayersideMessage.LOST};
				writePlayerside(sendBuffer, 1);
			}
			break;

			default : std::cout << "Default error: PlayersideWrite Dispatcher" << std::endl;
			break;
		}
}
