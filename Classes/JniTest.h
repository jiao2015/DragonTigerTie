#ifndef JNI_TEST_H
#define JNI_TEST_H

#include "cocos2d.h"

using namespace cocos2d;

void setPackageName(const char *packageName)
{
	CCLog("packageName: %s", packageName);	
}

void exitApp()
{
	CCDirector::sharedDirector()->end();
}
#endif