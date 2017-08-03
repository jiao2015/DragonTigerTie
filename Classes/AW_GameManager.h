#pragma once
#include "AW_MessageHead.h"

class CCPomeloEvent;
class GameManager
{
public:

	static GameManager *getInstance();
	void init();
	void addServerListener();
	void addOnServerCallback(const CCPomeloEvent& eventStruct);
	void clear();
	void removeCardItem(std::string _id);

	int _onlineCount = 0;
	PlayerInfo _playerInfo;
	LotteryResultSt _lastLotteryResult;

	Odds _odds;
	Odds _lastBetList;//为了显示上次没结算时，押注的三个数据
	Odds _currBetList;//当前结算时，押注的三个数据

	double _currCoin = 0;
	bool _isLockBet = false;
	double _winCoin = 0;
	bool _isBetCount = false;//是否押注了，没压住的话，没有onresult
	double _commissionCoin = 0;

    int _gameMusicIndex = 0;
	//std::string _expect = "";

	std::vector<HistoryStruct> _historyList;
	std::vector<CardStruct> _cardList;

	GameSetStruct _gameSetStruct;

private:

	static GameManager *_pInstance;
};
