//
//  GameTimer.cpp
//  KnightRPS
//
//  Created by jiaolong on 15/3/17.
//
//

#include "AW_GameTimer.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include <iomanip>
#include <memory>

GameTimer *GameTimer::create(TIME_TYPE type,int timeCount)
{
    GameTimer *pRet = new(std::nothrow) GameTimer();
    
    if (pRet && pRet->myInit(type,timeCount))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

GameTimer::GameTimer()
{
	_myScheduler = Director::getInstance()->getScheduler();
    
	_myScheduler->retain();
	_schedulerCallback = nullptr;
}

bool GameTimer::myInit(TIME_TYPE type,int timeCount)
{
	_myScheduler->schedule(std::bind(&GameTimer::myUpdata, this, std::placeholders::_1, type), this, timeCount, false, "what");
    
    //MainCityScene::getInstance()->stopTime(CC_CALLBACK_0(GameTimer::stopTime, this));
    
    return true;
}

void GameTimer::myUpdata(float time, TIME_TYPE type)
{
	_schedulerCallback(time, type);
}

void GameTimer::addGameTimer(schedulerCallback callback)
{
	_schedulerCallback = callback;
}

void GameTimer::stopTime()
{
    Director::getInstance()->getScheduler()->unschedule("what", this);
}