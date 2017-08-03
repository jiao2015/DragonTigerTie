#pragma once

#include "AW_MessageHead.h"

class HistoryItem :public Widget
{
public:

	virtual bool init(HistoryStruct historyStruct);
	static HistoryItem *create(HistoryStruct historyStruct);
	HistoryItem();
	void setWinImageOp(int result);

private:

	ImageView *_imageDragon;
	ImageView *_imageTie;
	ImageView *_imageTiger;

	Text *_textRound;
	Text* _textResult;
	Text* _textDragon;
	Text* _textTie;
	Text * _textTiger;

	Text* _textDragonPercent;
	Text* _textTiePercent;
	Text * _textTigerPercent;
};