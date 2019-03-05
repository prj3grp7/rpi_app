#include "WebPage.h"
#include "WebPageResponse.h"

WebPage::WebPage(MsgQueue * GameControllerMsgQueue)
	: GameControllerMsgQueue_(GameControllerMsgQueue)
{
}

void WebPage::run()
{
	WSInit();
	while (true)
	{
		if (h.listen(3000)) {
			h.run();
		}
	}
}

void WebPage::WSInit()
{
	h.onMessage([&](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {

		// Splitting string and storing substrings in vector
		ostringstream buffer;
		for (int i = 0; message[i] != '#'; i++)
			buffer << message[i];

		string gameInformation = buffer.str();

		char delimiter = '-';
		vector<string> tokens;

		istringstream str(gameInformation);
		for (string substring; getline(str, substring, delimiter); tokens.push_back(substring));

		//	WebPageResponse* ind = new WebPageResponse();

		//Setting team2
		team2_.setTeam(tokens.back());
		tokens.pop_back();
		team2_.setUser2(tokens.back());
		tokens.pop_back();
		team2_.setUser1(tokens.back());
		tokens.pop_back();

		// Setting team1
		team1_.setTeam(tokens.back());
		tokens.pop_back();
		team1_.setUser2(tokens.back());
		tokens.pop_back();
		team1_.setUser1(tokens.back());
		tokens.pop_back();

		// Sending to GameController class
		GameControllerMsgQueue_->send(ID_INFO_READY, new WebPageResponse(team1_, team2_));

		// Sending back to WS client
		ws->send(message, length, opCode);
	});
}
