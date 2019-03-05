#pragma once

#include <iostream>
#include <string>
#include <bitset>
#include <cstdint>
using namespace std;

class UserInfo
{
public:
	UserInfo(string username1 = "", string username2 = "", string team = "", uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
		: username1_(username1), username2_(username2), teamname_(team), red_(red), green_(green), blue_(blue) {
			cups_.reset();
	}

	//Inds�t copyconstructor
	UserInfo(const UserInfo & copyMe)
	{
		this->teamname_ = copyMe.teamname_;
		this->username1_ = copyMe.username1_;
		this->username2_ = copyMe.username2_;
		this->red_ = copyMe.red_;
		this->green_ = copyMe.green_;
		this->blue_ = copyMe.blue_;
	}

	UserInfo & operator=(const UserInfo & other){

		if(&other != this)
		{
		this->username1_ = other.username1_;
		this->username2_ = other.username2_;
		this->teamname_ = other.teamname_;
		this->red_ = other.red_;
		this->green_ = other.green_;
		this->blue_ = other.blue_;
		//this->cups_ = other.getCups();
		}
		return *this;

	}

	void printTeamInfo() const
	{
		cout << "Team is: " << endl;
		cout << "Teamname is: " << getTeam() << endl;
		cout << "Username1 is: " << getUser1() << endl;
		cout << "Username2 is: " << getUser2() << endl;
		cout << "Red colorcode: " << static_cast<unsigned int>(getRed()) << endl;
		cout << "Green colorcode: " << static_cast<unsigned int>(getGreen()) << endl;
		cout << "Blue colorcode: " << static_cast<unsigned int>(getBlue()) << endl << endl;
	}

	void setCups(const char * cupsArray)
	{
	cups_ = (bitset<6>)*cupsArray;
	}

	void setUser1(const string &user1)
	{
		username1_ = user1;
	}

	void setUser2(const string &user2)
	{
		username2_ = user2;
	}

	void setTeam(const string &team)
	{
		teamname_ = team;
	}

	bitset<6> getCups() const
	{
		return cups_;
	}

	const string getUser1() const
	{
		return username1_;
	}

	const string getUser2() const
	{
		return username2_;
	}

	const string getTeam() const
	{
		return teamname_;
	}

	void setRed(const string & red)
	{
		red_ = stoi(red);
	}

	void setGreen(const string & green)
	{
		green_ = stoi(green);
	}

	void setBlue(const string & blue)
	{

		blue_ = stoi(blue);
	}

	const uint8_t getRed() const
	{
		return red_;
	}

	const uint8_t getGreen() const
	{
		return green_;
	}

	const uint8_t getBlue() const
	{
		return blue_;
	}

	~UserInfo()
	{}

private:
	string username1_;
	string username2_;
	string teamname_;
	uint8_t red_;
	uint8_t green_;
	uint8_t blue_;
	bitset<6> cups_;
};
