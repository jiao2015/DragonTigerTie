#include "AW_GameManager.h"
#include "AW_MessageHead.h"
#include "PomeloWrapper.h"

GameManager* GameManager::_pInstance = nullptr;

GameManager *GameManager::getInstance()
{
	if (!_pInstance)
	{
		_pInstance = new GameManager();
		_pInstance->init();
	}

	return _pInstance;
}

void GameManager::clear()
{
	_isLockBet = false;
	_isBetCount = false;
	_lastBetList = Odds(0, 0, 0);
	_winCoin = 0;
	//PomeloInstance->removeAllListeners();
}

void GameManager::removeCardItem(std::string _id)
{
	for (auto it = _cardList.begin(); it != _cardList.end(); it++)
	{
		if ((*it)._id == _id)
		{
			_cardList.erase(it);
			break;
		}
	}
}

void GameManager::init()
{

}

void GameManager::addServerListener()
{
	PomeloInstance->addListener("onLotteryShowResult", CC_CALLBACK_1(GameManager::addOnServerCallback, this));
	PomeloInstance->addListener("onResult", CC_CALLBACK_1(GameManager::addOnServerCallback, this));
	PomeloInstance->addListener("onNotice", CC_CALLBACK_1(GameManager::addOnServerCallback, this));
	PomeloInstance->addListener("onChangeRate", CC_CALLBACK_1(GameManager::addOnServerCallback, this));
}

void GameManager::addOnServerCallback(const CCPomeloEvent& eventStruct)
{
	auto eventName = eventStruct.event;
	CCLOG("GameManager-eventName = %s", eventName.c_str());
	CCLOG("GameManager-evenCallback = %s", eventStruct.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(eventStruct.jsonMsg.c_str());

	if (d.HasParseError())
	{
	}
	else if (eventName == "onLotteryShowResult")
	{
		//_isLockBet = false;
		std::string opencode = d["openCode"].GetString();
		std::string opentime = d["openTime"].GetString();
		std::string expectnumber = d["expect"].GetString();
		int nextInterval = d["nextInterval"].GetInt();
		std::string nextexpect = d["nextExpect"].GetString();

		std::vector<std::string> openCodeList;
		
		std::stringstream ss;
		for (int i = 0; i < 5; i++)
		{
			ss.str("");
			char ffas = opencode.at(i * 2);
			ss << ffas;
			std::string asfd = ss.str();
			openCodeList.push_back(asfd);
		}

		int opentype = 0;
		int list0 = Value(openCodeList[0]).asInt();
		int list4 = Value(openCodeList[4]).asInt();

		if (list0 > list4)
		{
			opentype = 0;
		}
		else if (list0== list4)
		{
			opentype = 2;
		}
		else if (list0 < list4)
		{
			opentype = 1;
		}

		_odds = Odds(d["odds"]["tiger"].GetDouble(),
			d["odds"]["equal"].GetDouble(), d["odds"]["dragon"].GetDouble());

		LotteryResultSt lottery = LotteryResultSt(expectnumber, opentime, openCodeList, nextInterval, nextexpect);

		DisPatcher->dispatchCustomEvent("ShowResult", &lottery);

		if (!_isBetCount)
		{
			HistoryStruct historyStruct = HistoryStruct(d["expect"].GetString(), "0",
				"0", "0", __String::createWithFormat("%.2f", _odds.dragonOdd)->getCString(), __String::createWithFormat("%.2f", _odds.tieOdd)->getCString(),
				__String::createWithFormat("%.2f", _odds.tigerOdd)->getCString(), "0.00", Value(opentype).asString());

			_historyList.push_back(historyStruct);

			DisPatcher->dispatchCustomEvent("DisplayResult", &historyStruct);
		}

		
	}
	else if ("onResult" == eventName)
	{
		_winCoin = d["winLose"].GetDouble();
		_currCoin = d["coin"].GetDouble();

		/*
		dateString("")
		, betDraCountString("")
		, betTieCountString("")
		, betTigCountString("")
		, betDraPercent("")
		, betTiePercent("")
		, betTigPercent("")
		, resultCount("")
		, result("")
		*/
		HistoryStruct historyStruct = HistoryStruct(d["expect"].GetString(), __String::createWithFormat("%.2f", d["betDragon"].GetDouble())->getCString(),
			__String::createWithFormat("%.2f", d["betEqual"].GetDouble())->getCString(), __String::createWithFormat("%.2f", d["betTiger"].GetDouble())->getCString(),
			__String::createWithFormat("%.2f", _odds.dragonOdd)->getCString(), __String::createWithFormat("%.2f", _odds.tieOdd)->getCString(),
			__String::createWithFormat("%.2f", _odds.tigerOdd)->getCString(),
			__String::createWithFormat("%.2f", _winCoin)->getCString(), Value(d["openType"].GetInt()).asString());

		_historyList.push_back(historyStruct);

		DisPatcher->dispatchCustomEvent("DisplayResult", &historyStruct);
	}
	else if ("onNotice" == eventName)
	{
		std::string msg = d["msg"].GetString();

		DisPatcher->dispatchCustomEvent("ReceiveNotice",&msg);
	}
	else if ("onChangeRate" == eventName)
	{
		_odds = Odds(d["tiger"].GetDouble(), d["equal"].GetDouble(), d["dragon"].GetDouble());

		DisPatcher->dispatchCustomEvent("ChangeOdds");
	}
}
