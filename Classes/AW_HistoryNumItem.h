#pragma once

#include "AW_MessageHead.h"

class HistoryNumItem :public Widget
{
public:

	virtual bool init(HisNumStruct historyStruct);
	static HistoryNumItem *create(HisNumStruct historyStruct);
	HistoryNumItem();

private:

	Text *_textRound;
	Text* _textNumber;

	Sprite *_spType;
};