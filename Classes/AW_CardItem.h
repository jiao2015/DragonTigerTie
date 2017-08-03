#pragma once

#include "AW_MessageHead.h"

class CCPomeloRequestResult;
class CardItem :public Layout
{
public:

	CardItem();
	CREATE_FUNC(CardItem);

	virtual bool init();
	void touchEventCallback(Ref *pSender, Widget::TouchEventType type);
	void deleteCardCallback(const CCPomeloRequestResult& result);

	void setProfile(CardStruct cardStruct);
	CardStruct getCardStruct() { return _cardStruct; }

private:

	Layout *_layoutBg;

	Text *_textCard;
	Text* _textBank;
	Text *_textName;
	CardStruct _cardStruct;

	cocos2d::Vec2 _getStartPos;
	cocos2d::Vec2 _getEndPos;

	int timeInv;
};