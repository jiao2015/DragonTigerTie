#pragma once

#include "cocos2d.h"
#include <string>
#include "ui\CocosGUI.h"
#include "editor-support\cocostudio\CocoStudio.h"
//////
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "json/rapidjson.h"
#include "AW_UnicodeGB2312.h"
#include "AW_SoundManger.h"
//////

USING_NS_CC;
using namespace cocos2d::ui;

const float TapLayerMoveTime = 0.2f;
const float SetLayerMoveTime = 0.5f;
const int StartCountTime = 5;
const int RandomMusicIndex = 7;

const Vec2 TapLayerMoveVec = Vec2(0, 1280);
const Size CardItemSize = Size(720, 150);

const std::string IP_NW = "192.168.0.214";
const std::string IP_WW = "115.126.93.245";
const std::string IP_DOWN_PNG = "http://192.168.0.182:2000";
//const std::string IP_NW = "118.190.41.142";

#define G2U		UnicodeGB2312::G2U
#define U2G		UnicodeGB2312::U2G
#define DisPatcher					Director::getInstance()->getEventDispatcher()
#define GameInstance			GameManager::getInstance()
#define UserInstance				UserDefault::getInstance()
#define PomeloInstance			CCPomeloWrapper::getInstance()
#define UserInstance				UserDefault::getInstance()
#define AW_INIT_IF(cond)           if(cond) return false

#define SetLocalMusic(isOn)			UserDefault::getInstance()->setBoolForKey("MusicSwitch", isOn)
#define SetLocalSound(isOn)			UserDefault::getInstance()->setBoolForKey("SoundSwitch", isOn)
#define GetLocalSound			UserDefault::getInstance()->getBoolForKey("SoundSwitch")
#define GetLocalMusic			UserDefault::getInstance()->getBoolForKey("MusicSwitch")

enum REGEX_TYPE
{
	REGEX_PHONE = 0,
	REGEX_CODE,
	REGEX_MSG
};

enum TIME_TYPE
{
	TIME_0 = 0,
	TIME_1,
	TIME_2,
	TIME_3,
	TIME_4,
	TIME_5,
	TIME_6,
	TIME_7
};

struct GameSetStruct
{
	GameSetStruct(float commissioncharge, float commissionlimit, std::vector<int> commissionfreeday, int betclosetime)
		:
		commissionCharge(commissioncharge)
		,commissionLimit(commissionlimit)
		, commissionFreeDay(commissionfreeday)
		, betCloseTime(betclosetime)
	{

	}

	GameSetStruct()
		:
		commissionCharge(0)
		, commissionLimit(0)
		, commissionFreeDay(std::vector<int>())
		, betCloseTime(0)
	{

	}

	float commissionCharge;//佣金手续费百分比 
	float commissionLimit;//佣金限制 ，是押注额的多少倍 
	std::vector<int> commissionFreeDay;// 佣金提现免费日 
	int betCloseTime; // 倒计时多少秒 关闭押注 
};

struct CardStruct
{
	CardStruct(std::string username, std::string cardnumber, std::string cardbank, std::string cardopenbank,std::string id1)
		:
		userName(username)
		, cardNumber(cardnumber)
		, cardBank(cardbank)
		, cardOpenBank(cardopenbank)
		, _id(id1)
	{

	}

	CardStruct()
		:
		userName("")
		, cardNumber("")
		, cardBank("")
		, cardOpenBank("")
		, _id("")
	{

	}

	std::string userName;
	std::string cardNumber;
	std::string cardBank;
	std::string cardOpenBank;
	std::string _id;
};
struct RegisterInfo
{
	RegisterInfo()
		:username("")
		, password("")
		, nickname("")
	{

	}

	RegisterInfo(std::string username1, std::string password1, std::string nickname1)
		:username(username1)
		,password(password1)
		,nickname(nickname1)
	{

	}

	std::string username;
	std::string password;
	std::string nickname;
};

struct Odds
{
	Odds()
		:
		tigerOdd(0)
		, tieOdd(0)
		, dragonOdd(0)
	{

	}

	Odds(double tiger, double tie, double dragon)
		:
		tigerOdd(tiger)
		, tieOdd(tie)
		, dragonOdd(dragon)
	{

	}

	double tigerOdd;
	double tieOdd;
	double dragonOdd;
};

struct BetStruct
{
	BetStruct()
		:
		betCount("")
		, betType("")
	{

	}

	BetStruct(std::string betcount, std::string bettype)
		:
		betCount(betcount)
		, betType(bettype)
	{

	}

	std::string betCount;
	std::string betType;
};

struct HisNumStruct
{
	HisNumStruct(std::string datestring, std::string prizenumber, std::string result1)
		:
		dateString(datestring)
		, prizeNumber(prizenumber)
		, result(result1)
	{

	}

	HisNumStruct()
		:
		dateString("")
		, prizeNumber("")
		, result("")
	{

	}

	std::string dateString;
	std::string prizeNumber;
	std::string result;
};

struct HistoryStruct
{
	HistoryStruct(std::string datestring, std::string betdracountstring, std::string bettiecountstring, 
		std::string bettigcountstring, std::string betdrapercent, std::string bettiepercent, 
		std::string bettigpercent, std::string resultcount, std::string result1)
		:
		dateString(datestring)
		, betDraCountString(betdracountstring)
		, betTieCountString(bettiecountstring)
		, betTigCountString(bettigcountstring)
		, betDraPercent(betdrapercent)
		, betTiePercent(bettiepercent)
		, betTigPercent(bettigpercent)
		, resultCount(resultcount)
		, result(result1)
	{

	}

	HistoryStruct()
		:
		dateString("")
		, betDraCountString("")
		, betTieCountString("")
		, betTigCountString("")
		, betDraPercent("")
		, betTiePercent("")
		, betTigPercent("")
		, resultCount("")
		, result("")
	{

	}

	std::string dateString;

	std::string betDraCountString;
	std::string betTieCountString;
	std::string betTigCountString;

	std::string betDraPercent;
	std::string betTiePercent;
	std::string betTigPercent;

	std::string resultCount;
	std::string result;
};

struct LotteryResultSt
{
	LotteryResultSt(std::string expectnumber, std::string opentime, std::vector<std::string> opencode, int nextinterval, std::string nextexpect)
		:
		openTime(opentime)
		, expectNumber(expectnumber)
		, openCode(opencode)
		, nextInterval(nextinterval)
		, nextExpect(nextexpect)
	{
	}

	LotteryResultSt()
		:
		expectNumber("")
		, openTime("")
		, openCode(std::vector<std::string>())
		, nextInterval(0)
		, nextExpect("")
	{
	}

	std::string expectNumber;
	std::string openTime;
	std::vector<std::string> openCode;
	int nextInterval;
	std::string nextExpect;
};

struct PlayerInfo
{
	PlayerInfo()
		:
		uid("")
		, nickName("")
		, icon("")
		, coin(0)
	{

	}

	PlayerInfo(std::string uid1, std::string nickname, std::string icon1, double coin1)
		:
		uid(uid1)
		,nickName(nickname)
		, icon(icon1)
		, coin(coin1)
	{

	}
	std::string uid;
	std::string nickName;
	std::string icon;
	double coin;
};
