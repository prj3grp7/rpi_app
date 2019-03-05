#include "PlayersideRead.h"

PlayersideRead::PlayersideRead(const char * filename, MsgQueue * GameControllerQueue, int playerside)
	: GameControllerQueue_(GameControllerQueue), playerside_(playerside)
{
	filePointer_ = open(filename, O_RDWR);
	if (filePointer_ == -1) {
		cout << "PS" << playerside_ << "ReadOpen: Failed to open with err: " << strerror(errno) << endl << endl;
	}
}

void PlayersideRead::translateMessage()
{
	PlayersideResponse * msg = new PlayersideResponse(readBuffer_, playerside_);
	GameControllerQueue_->send(CUPS, msg);
}

int PlayersideRead::readPlayerside()
{
	cout << "PlayersideRead" << playerside_;
	readI2C();
}

const char * PlayersideRead::getPlayersideReadBuffer()
{
	return getReadBuffer();
}

void PlayersideRead::run()
{
	while (true)
	{
		int num_byte = readPlayerside();
		//If something has been received
		if (num_byte > 0)
		{
			translateMessage();
		}
	}
}
