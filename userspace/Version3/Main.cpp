#include "GameController.h"
#include "PlayersideRead.h"
#include "PlayersideWrite.h"
#include "BallDispenserRead.h"
#include "BallDispenserWrite.h"

MsgQueue GameControllerMsgQueue_; //Allokér dynamisk fremover

int main(int argc, char * argv[])
{
	//Shared GameController Queue between all classes

	//Playerside Read and Write initialization
	PlayersideRead ps1_r("/dev/playerside1", &GameControllerMsgQueue_, 1);
	PlayersideRead ps2_r("/dev/playerside2", &GameControllerMsgQueue_, 2);

	PlayersideWrite ps1_w("/dev/playerside1", 1);
	PlayersideWrite ps2_w("/dev/playerside2", 2);

	BallDispenserRead bd_r("/dev/balldispenser", &GameControllerMsgQueue_);
	BallDispenserWrite bd_w("/dev/balldispenser");


//GameController initialization
	GameController GC(&ps1_w, &ps2_w, &bd_w, &GameControllerMsgQueue_);

	//Thread initialization
	Thread GameThread(&GC);

	Thread Playerside1ReadThread(&ps1_r);
	Thread Playerside1WriteThread(&ps1_w);

	Thread Playerside2ReadThread(&ps2_r);
	Thread Playerside2WriteThread(&ps2_w);

	Thread BallDispenserReadThread(&bd_r);
	Thread BallDispenserWriteThread(&bd_w);


	//Create / Start threads
	GameThread.start();

	Playerside1ReadThread.start();
	Playerside1WriteThread.start();

	Playerside2ReadThread.start();
	Playerside2WriteThread.start();

	BallDispenserReadThread.start();
	BallDispenserWriteThread.start();

	//Join Threads
	GameThread.join();

	Playerside1ReadThread.join();
	Playerside1WriteThread.join();

	Playerside2ReadThread.join();
	Playerside2WriteThread.join();

	BallDispenserReadThread.join();
	BallDispenserWriteThread.join();

	return 0;

}
