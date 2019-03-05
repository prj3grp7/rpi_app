#pragma once
#include "Message.h"
#include "GameControllerRequest.h"
#include "Thread.h"
#include "ThreadFunctor.h"
#include "PlayersideRead.h"
#include "PlayersideWrite.h"
#include "BallDispenserResponse.h"
#include "EndGameCfm.h"
#include "WebPageResponse.h"
#include "BallDispenserWrite.h"

class GameController : public ThreadFunctor
{
public:
	GameController(PlayersideWrite * ps1_w, PlayersideWrite * ps2_w, BallDispenserWrite * bd_w, MsgQueue * GameController)
		: PlayersideWriteObj1_(ps1_w), PlayersideWriteObj2_(ps2_w), BallDispenserWriteObj_(bd_w), GameControllerMsgQueue_(GameController)
		{
			WebPage_Information = false; 
		}
	~GameController();

	//Thread control for Playerside
	virtual void run();


protected:
	enum GameControllerState
	{
		GAME_STATE_IDLE,
		GAME_STATE_STARTING,
		GAME_STATE_PLAYING,
		GAME_STATE_ENDGAME,
		GAME_STATE_SERVICE,
	};
	GameControllerState state = GAME_STATE_IDLE;
	MsgQueue * GameControllerMsgQueue_;

private:
	//Dispatcher
	void dispatcherGameMessage(unsigned long msgID, Message * msg);

	//Overall Event handlers:
	void handleSystemStart();
	void handleSystemPlaying();
	void handleSystemEndGame(EndGameCfm * msg);
	void handleSystemService(BallDispenserResponse * msg);

	//Boundary class communication handlers:
	void handlePlayersideResponse(PlayersideResponse * msg);
	void handleIdleRequest();
	void handleBallDispenserResponse(BallDispenserResponse * msg);
	void handleWebPageResponse(WebPageResponse * msg);

	UserInfo team1_;
	UserInfo team2_;

	PlayersideWrite * PlayersideWriteObj1_;
	PlayersideWrite * PlayersideWriteObj2_;
	BallDispenserWrite * BallDispenserWriteObj_;

	bool WebPage_Information;

};
