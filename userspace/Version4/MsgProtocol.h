#pragma once

//Storage om I2C protocol messages:

//Rpi to Playersides
struct PlayersideMsg{
	const char IDLE = 0b00001010;
	const char STARTING = 0b00001011;
	const char PLAYING = 0b00001100;
	const char LOST = 0b00001101;
	const char WON = 0b00001110;
};
static struct PlayersideMsg PlayersideMessage;

//Balldispenser to RPi
struct BallDispenserMsg{
	const char COIN_INSERTED = 0b00011110;
	const char EMPTY = 0b00011111;
	const char NOT_EMPTY = 0b00100000;
};
static struct BallDispenserMsg BallDispenserMessage;

//RPi to BallDispenser
struct DispenseMsg{
	const char ON = 0b00010100;
	const char OFF = 0b00010101;
};
static struct DispenseMsg DispenseMessage;


//MSG IDs:

enum BallDispenser{
	NEW_INFO = 121,
	ON = 119,
	OFF = 120,
};

//RPi to Display
enum DisplayShow{
	_IDLE = 103,
  _PLACE_CUPS = 104,
	_INFO = 105,
  _GAME_STATUS = 106,
	_ENDGAME = 107,
	_SERVICE = 108,
	_CUPS_UPDATED = 118,
};

//Internal messages for GameController
//Simply to initialize each state for GameController
enum SYSTEM_STATE{
  IDLE = 109,
	STARTING = 110,
	PLAYING = 111,
	LOST = 112,
	WON = 113,
	ENDGAME = 114,
	SERVICE = 115,
};

//Playerside to Rpi
enum {
	CUPS = 116,
};

//WebPage to RPi
enum WebPageServer {
	ID_INFO_READY = 117,
};
