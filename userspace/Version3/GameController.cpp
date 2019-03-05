#include "GameController.h"

GameController::~GameController()
{
}

void GameController::dispatcherGameMessage(unsigned long msgID, Message * msg)
{
	switch (state)
	{
	case GAME_STATE_IDLE:
	{
		switch (msgID)
		{
		case SYSTEM_STATE::IDLE:
		{
			cout << "SYSTEM_STATE::IDLE" << endl;
			handleIdleRequest();
		}
		break;

		case BallDispenser::NEW_INFO:
			cout << "NEW_INFO" << endl;
			handleBallDispenserResponse(static_cast<BallDispenserResponse *>(msg));
		break;

		case WebPage::ID_INFO_READY :
		handleWebPageResponse(static_cast<WebPageResponse *>(msg));
		break;
		}
	}
	break;

	case GAME_STATE_STARTING:
	{
		switch (msgID)
		{
		case SYSTEM_STATE::STARTING:
		{
			cout << "SYSTEM_STATE::STARTING" << endl;
			handleSystemStart();
		}
		break;

		case CUPS :
		{
			cout << "CUPS, STARTING!" << endl;
			handlePlayersideResponse(static_cast<PlayersideResponse *>(msg));
		}
		break;

		case WebPage::ID_INFO_READY :
			handleWebPageResponse(static_cast<WebPageResponse *>(msg));
		break;

		default : cout << "GAME_STATE_STARTING DEFUALT WAS CALLED" << endl;

		} //End GAME_STATE_STARTING switch
	}
	break;

	case GAME_STATE_PLAYING :
	{
		switch (msgID)
		{
		case SYSTEM_STATE::PLAYING :
		{
			handleSystemPlaying();
		}
		break;

		case CUPS :
		{
			cout << "CUPS, PlAYING!" << endl;
			handlePlayersideResponse(static_cast<PlayersideResponse *>(msg));
		}
		break;
		} //Switch case end
	}
	break;

	case GAME_STATE_ENDGAME :
	{
		if (msgID == SYSTEM_STATE::ENDGAME)
		{
			handleSystemEndGame(static_cast<EndGameCfm *>(msg));
		}
	}
	break;

	case GAME_STATE_SERVICE :
	{
		if(msgID == SYSTEM_STATE::SERVICE)
		handleSystemService(static_cast<BallDispenserResponse *>(msg));
	}
	break;

	} //Main switch case

}

void GameController::handleBallDispenserResponse(BallDispenserResponse * msg)
{

	if(msg->readPtr_[0] == BallDispenserMessage.EMPTY)
	{
		//DisplayObj_->DisplayMsgQueue.send(Display::SHOW_SERVICE);
		this->state = GAME_STATE_SERVICE;
		GameControllerMsgQueue_->send(SYSTEM_STATE::SERVICE);
	}

	else if(msg->readPtr_[0] == BallDispenserMessage.NOT_EMPTY && this->state == GAME_STATE_SERVICE)
	{
		this->state = GAME_STATE_IDLE;
		GameControllerMsgQueue_->send(SYSTEM_STATE::IDLE);
	}

	if (msg->readPtr_[0] == BallDispenserMessage.COIN_INSERTED && this->state == GAME_STATE_IDLE)
	{
		cout << "COIN_INSERTED" << endl;
		this->state = GAME_STATE_STARTING;
		GameControllerMsgQueue_->send(SYSTEM_STATE::STARTING);
	}
}

void GameController::handleWebPageResponse(WebPageResponse * msg)
{
	cout << "HandleWebPageResponse" << endl;
		//Mangler handler for WebPage
		if(msg != nullptr && this->state == GAME_STATE_STARTING || this->state == GAME_STATE_IDLE)
		{
		this->team1_ = msg->team1_;
		this->team2_ = msg->team2_;
		}

		if(this->team1_.getCups().all() && this->team2_.getCups().all() && this->state == GAME_STATE_STARTING)
		{
		this->state = GAME_STATE_PLAYING;
		GameControllerMsgQueue_->send(SYSTEM_STATE::PLAYING);
		}
}

void GameController::handlePlayersideResponse(PlayersideResponse * msg)
{
	cout << "HandlePlayersideResonse" << endl;

	if(msg->playerside_ == 1)
	{
	team1_.setCups(msg->readPtr_);
	}

	else if(msg->playerside_ == 2)
	{
	team2_.setCups(msg->readPtr_);
	}
	//DisplayObj_->DisplayMsgQueue_.send(Display::CUPS_UPDATED);

	cout << team1_.getCups() << endl;
	cout << team2_.getCups() << endl;

	if(team1_.getCups().all() && team2_.getCups().all() && this->state == GAME_STATE_STARTING)
	{

		if(WebPage_Information){
			GameControllerMsgQueue_->send(WebPage::ID_INFO_READY);
		}
	}

	else if(team1_.getCups().none() || team2_.getCups().none() && this->state == GAME_STATE_PLAYING)
	{
		this->state = GAME_STATE_ENDGAME;
		if(team1_.getCups().none())
			GameControllerMsgQueue_->send(SYSTEM_STATE::ENDGAME, new EndGameCfm(1));

		else if(team2_.getCups().none())
			GameControllerMsgQueue_->send(SYSTEM_STATE::ENDGAME, new EndGameCfm(2));
	}
}

void GameController::handleIdleRequest()
{
	WebPage_Information == false;
	PlayersideWriteObj1_->PlayersideMsgQueue_.send(SYSTEM_STATE::IDLE);
	PlayersideWriteObj2_->PlayersideMsgQueue_.send(SYSTEM_STATE::IDLE);
	BallDispenserWriteObj_->BallDispenserMsgQueue_.send(BallDispenser::ON);
	//DisplayObj_->DisplayMsgQueue_.send(Display::SHOW_IDLE);


	//TIL TESTNING!!!
	GameControllerMsgQueue_->send(BallDispenser::NEW_INFO, new BallDispenserResponse(&BallDispenserMessage.COIN_INSERTED));
}

void GameController::run()
{
	GameControllerMsgQueue_->send(SYSTEM_STATE::IDLE);
	while (true)
	{
		unsigned long id;
		Message * msg = GameControllerMsgQueue_->receive(id);
		dispatcherGameMessage(id, msg);
		delete msg;
	}
}

void GameController::handleSystemStart()
{
	PlayersideWriteObj1_->PlayersideMsgQueue_.send(SYSTEM_STATE::STARTING);
	PlayersideWriteObj2_->PlayersideMsgQueue_.send(SYSTEM_STATE::STARTING);
	BallDispenserWriteObj_->BallDispenserMsgQueue_.send(BallDispenser::OFF);
	//DisplayObj_->DisplayMsgQueue_.send(Display::SHOW_PLACE_CUPS);

	//TIL TESTNING
}

void GameController::handleSystemPlaying()
{
	PlayersideWriteObj1_->PlayersideMsgQueue_.send(SYSTEM_STATE::PLAYING);
	PlayersideWriteObj2_->PlayersideMsgQueue_.send(SYSTEM_STATE::PLAYING);
	//DisplayObj_->DisplayMsgQueue_.send(Display::SHOW_GAME_STATUS);
}

void GameController::handleSystemEndGame(EndGameCfm * cfm)
{
	if (cfm->playerside_ == 1)
	{
		PlayersideWriteObj1_->PlayersideMsgQueue_.send(SYSTEM_STATE::LOST);
		PlayersideWriteObj2_->PlayersideMsgQueue_.send(SYSTEM_STATE::WON);
		//DisplayObj_->DisplayMsgQueue_.send(Display::SHOW_ENDGAME, new EndGameCfm(1));
	}
	else if (cfm->playerside_ == 2)
	{
		PlayersideWriteObj1_->PlayersideMsgQueue_.send(SYSTEM_STATE::WON);
		PlayersideWriteObj2_->PlayersideMsgQueue_.send(SYSTEM_STATE::LOST);
		//DisplayObj_->DisplayMsgQueue_.send(Display::SHOW_ENDGAME, new EndGameCfm(2));
	}

	sleep(10); //Timer NEW_INFO

	this->state = GAME_STATE_IDLE;
	GameControllerMsgQueue_->send(SYSTEM_STATE::IDLE);
}

void GameController::handleSystemService(BallDispenserResponse * msg)
{
	BallDispenserWriteObj_->BallDispenserMsgQueue_.send(BallDispenser::OFF);
}
