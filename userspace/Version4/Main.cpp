#include "GameController.h"
#include "PlayersideRead.h"
#include "PlayersideWrite.h"
#include "BallDispenserRead.h"
#include "BallDispenserWrite.h"
#include "WebPage.h"

MsgQueue GameControllerMsgQueue_; //Allokér dynamisk fremover

int main(int argc, char * argv[])
{

	//WebPage initialization
	WebPage WebPageObj(&GameControllerMsgQueue_);

	//Playerside Read and Write initialization
	PlayersideRead ps1_r("/dev/playerside1", &GameControllerMsgQueue_, 1);
	PlayersideRead ps2_r("/dev/playerside2", &GameControllerMsgQueue_, 2);

	PlayersideWrite ps1_w("/dev/playerside1", 1);
	PlayersideWrite ps2_w("/dev/playerside2", 2);

	BallDispenserRead bd_r("/dev/BallDispenser", &GameControllerMsgQueue_);
	BallDispenserWrite bd_w("/dev/BallDispenser");


	//GameController initialization
	GameController GC(&ps1_w, &ps2_w, &bd_w, &GameControllerMsgQueue_);

	//Display initialization
	//Display_Functor DisplayObj_(GC.getTeam1(), GC.getTeam2());

	//GC.setDisplayMsgQueue(DisplayObj_.getMsgQueue());


	//Thread initialization
	Thread GameThread(&GC);

	Thread Playerside1ReadThread(&ps1_r);
	Thread Playerside1WriteThread(&ps1_w);

	Thread Playerside2ReadThread(&ps2_r);
	Thread Playerside2WriteThread(&ps2_w);

	Thread WebPageThread(&WebPageObj);

	Thread BallDispenserReadThread(&bd_r);
	Thread BallDispenserWriteThread(&bd_w);


	//Create / Start threads
	GameThread.start();

	BallDispenserReadThread.start();
	BallDispenserWriteThread.start();

	Playerside1ReadThread.start();
	Playerside1WriteThread.start();

	Playerside2ReadThread.start();
	Playerside2WriteThread.start();

	WebPageThread.start();

	//Join Threads
	GameThread.join();

	Playerside1ReadThread.join();
	Playerside1WriteThread.join();


	Playerside2ReadThread.join();
	Playerside2WriteThread.join();

	WebPageThread.join();

	BallDispenserReadThread.join();
	BallDispenserWriteThread.join();

	return 0;

}
