#pragma once
#include <stdio.h>
#include "PomeloWrapper.h"
#include "cocos2d.h"
#include "AW_MessageHead.h"

using namespace cocos2d;

typedef std::function<void (float,TIME_TYPE)> schedulerCallback;

class GameTimer: public Ref
{
public:
    
    static GameTimer *create(TIME_TYPE type, int timeCount);

    bool myInit(TIME_TYPE type,int timeCount);
    
    void myUpdata(float time, TIME_TYPE type);
    void addGameTimer(schedulerCallback callback);
    void stopTime();
    
    GameTimer();
    
private:
    
    Scheduler *_myScheduler;
    schedulerCallback _schedulerCallback;
};
