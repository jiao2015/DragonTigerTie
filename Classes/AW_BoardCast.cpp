#include "AW_BoardCast.h"
#include "AW_UnicodeGB2312.h"
#include "AW_GameManager.h"

BoardCast::BoardCast()
	:
	_text(nullptr)
	, _isStop(false)
{


}

bool BoardCast::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!Layer::init());

		Size visibleSize = Director::getInstance()->getVisibleSize();
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui_serverlist.plist");

		//水平滚动公告
		Sprite* textbase = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("login_textbase.png"));
		textbase->setPosition(Vec2(visibleSize.width / 2, 50));
		textbase->setScaleX(8.0f);
		this->addChild(textbase);
		_text = LabelTTF::create("", "Arial", 30);
        
		std::string text1 = "";

		for (auto it = GameInstance->_gameSetStruct.commissionFreeDay.begin(); it != GameInstance->_gameSetStruct.commissionFreeDay.end(); it++)
		{
			text1 += Value((*it)).asString() + "日  ";
		}

		std::string text = "提示：1.佣金提余额手续费收 " + 
			std::string(__String::createWithFormat("%.2f", GameInstance->_gameSetStruct.commissionCharge * 100)->getCString()) + "%.   "
			+ "2.佣金提余额的上限为当月押注额的 " + std::string(__String::createWithFormat("%.1f", GameInstance->_gameSetStruct.commissionLimit)->getCString())
			+ " 倍.   " + "3.佣金提余额免费日为 " + text1 + "  " + "4.倒计时 " + Value(GameInstance->_gameSetStruct.betCloseTime).asString() + " 秒内关闭押注";

		_text->setString(G2U(text.c_str()));
		_text->setPosition(Vec2(visibleSize.width + _text->getContentSize().width / 2, 50));
		_text->setColor(Color3B(255, 0, 0));
		
		this->addChild(_text);

		this->scheduleUpdate();
		//this->setScale(1.5f);

		bRet = true;
	} while (0);
	return bRet;
}

void BoardCast::update(float delta)
{
	if (_isStop)
	{
		return;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	int newX = _text->getPositionX() - 1;
	if (newX <= -_text->getContentSize().width / 2)
	{
		newX = visibleSize.width + _text->getContentSize().width / 2;
	}
	_text->setPositionX(newX);
}

void BoardCast::stop()
{
	_isStop = true;
	this->setVisible(false);
}

void BoardCast::start()
{
	_isStop = false;
	this->setVisible(true);
	_text->setPosition(Vec2(Director::getInstance()->getVisibleSize().width + _text->getContentSize().width / 2, 50));
}

void BoardCast::showBoardCast(std::string msg)
{
	//Size visibleSize = Director::sharedDirector()->getVisibleSize();
	_text->setString(G2U(msg.c_str()));
	//_text->setPosition(ccp(visibleSize.width + _text->getContentSize().width / 2, 50));
}
