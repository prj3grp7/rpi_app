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
			cout << "NEW_INFO : ";
			handleBallDispenserResponse(static_cast<BallDispenserResponse *>(msg));
		break;

		case WebPageServer::ID_INFO_READY :
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

		case WebPageServer::ID_INFO_READY :
			handleWebPageResponse(static_cast<WebPageResponse *>(msg));
		break;

		default : cout << "GAME_STATE_STARTING DEFUALT WAS CALLED" << endl;
		break;

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

		default : cout << "Message received was out of bounds" << endl;
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

void GameController::handleWebPageResponse(WebPageResponse * msg)
{
	cout << "HandleWebPageResponse" << endl;
		if(msg != nullptr && (this->state == GAME_STATE_STARTING || this->state == GAME_STATE_IDLE))
		{
		this->team1_ = msg->team1_;
		this->team2_ = msg->team2_;
		team1_.printTeamInfo();
		team2_.printTeamInfo();
		WebPage_Information = true;
		}

		if(this->team1_.getCups().all() && this->team2_.getCups().all() && this->state == GAME_STATE_STARTING && WebPage_Information)
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

	cout << "TEAM1: "<< team1_.getCups() << endl;
	cout << "TEAM2: "<< team2_.getCups() << endl;

	if(team1_.getCups().all() && team2_.getCups().all() && (this->state == GAME_STATE_STARTING))
	{
		if(WebPage_Information){
			GameControllerMsgQueue_->send(WebPageServer::ID_INFO_READY);
		}
	}

	else if((team1_.getCups().none() || team2_.getCups().none()) && (this->state == GAME_STATE_PLAYING))
	{
		this->state = GAME_STATE_ENDGAME;
		if(team1_.getCups().none())
			GameControllerMsgQueue_->send(SYSTEM_STATE::ENDGAME, new EndGameCfm(1));

		else if(team2_.getCups().none())
			GameControllerMsgQueue_->send(SYSTEM_STATE::ENDGAME, new EndGameCfm(2));
	}

}

void GameController::handleBallDispenserResponse(BallDispenserResponse * msg)
{

	if(msg->readPtr_[0] == BallDispenserMessage.EMPTY)
	{
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

void GameController::handleIdleRequest()
{
	WebPage_Information = false;
	PlayersideWriteObj1_->PlayersideMsgQueue_.send(SYSTEM_STATE::IDLE);
	PlayersideWriteObj2_->PlayersideMsgQueue_.send(SYSTEM_STATE::IDLE);
	BallDispenserWriteObj_->BallDispenserMsgQueue_.send(BallDispenser::ON);
	//DisplayObj_->DisplayMsgQueue_.send(Display::SHOW_IDLE);

	//TIL TESTNING
	//GameControllerMsgQueue_->send(BallDispenser::NEW_INFO, new BallDispenserResponse(&BallDispenserMessage.COIN_INSERTED));

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
}

void GameController::handleSystemPlaying()
{
	TeamColorsMsg * team1 = new TeamColorsMsg(team1_.getRed(), team1_.getGreen(), team1_.getBlue(), team2_.getRed(), team2_.getGreen(), team2_.getBlue());
	TeamColorsMsg * team2 = new TeamColorsMsg(team2_.getRed(), team2_.getGreen(), team2_.getBlue(), team1_.getRed(), team1_.getGreen(), team1_.getBlue());

	PlayersideWriteObj1_->PlayersideMsgQueue_.send(SYSTEM_STATE::PLAYING, team1);
	PlayersideWriteObj2_->PlayersideMsgQueue_.send(SYSTEM_STATE::PLAYING, team2);
	//DisplayObj_->DisplayMsgQueue_.send(Display::SHOW_GAME_STATUS);

}

void GameController::handleSystemEndGame(EndGameCfm * cfm)
{
	if (cfm->playerside_ == 1)
	{
		PlayersideWriteObj1_->PlayersideMsgQueue_.send(SYSTEM_STATE::LOST);
		PlayersideWriteObj2_->PlayersideMsgQueue_.send(SYSTEM_STATE::WON);
		BallDispenserWriteObj_->BallDispenserMsgQueue_.send(BallDispenser::ON);
		//DisplayObj_->DisplayMsgQueue_.send(Display::SHOW_ENDGAME, new EndGameCfm(1));
	}
	else if (cfm->playerside_ == 2)
	{
		PlayersideWriteObj1_->PlayersideMsgQueue_.send(SYSTEM_STATE::WON);
		PlayersideWriteObj2_->PlayersideMsgQueue_.send(SYSTEM_STATE::LOST);
		BallDispenserWriteObj_->BallDispenserMsgQueue_.send(BallDispenser::ON);
		//DisplayObj_->DisplayMsgQueue_.send(Display::SHOW_ENDGAME, new EndGameCfm(2));
	}

	sleep(5);
	this->state = GAME_STATE_IDLE;
	GameControllerMsgQueue_->send(SYSTEM_STATE::IDLE);
}

void GameController::handleSystemService(BallDispenserResponse * msg)
{
	cout << "SERVICE STATE INITIATED" << endl;
	BallDispenserWriteObj_->BallDispenserMsgQueue_.send(BallDispenser::OFF);
}
