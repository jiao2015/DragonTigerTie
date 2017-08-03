#include "AW_HistoryNumItem.h"
#include "AW_UnicodeGB2312.h"

HistoryNumItem *HistoryNumItem::create(HisNumStruct historyStruct)
{
	auto history = new HistoryNumItem();
	if (history && history->init(historyStruct))
	{
		history->autorelease();
		return history;
	}
	return nullptr;
}

HistoryNumItem::HistoryNumItem()
	:
	_textRound(nullptr)
	,_textNumber(nullptr)
	, _spType(nullptr)
{

}

bool HistoryNumItem::init(HisNumStruct historyStruct)
{
	if (!Widget::init())
	{
		return false;
	}
	
	auto historyNode = CSLoader::createNode("hisNumItem.csb");
	this->setContentSize(historyNode->getContentSize());
	this->setAnchorPoint(Vec2(0, 1));
	this->addChild(historyNode);

	_textRound = historyNode->getChildByName<Text *>("Text_round");
	_textNumber = historyNode->getChildByName<Text *>("Text_number");

	_spType = historyNode->getChildByName<Sprite *>("Sprite_type");

	int result = Value(historyStruct.result).asInt();

	std::string textround = "第 " + historyStruct.dateString + " 期";
	_textRound->setString(G2U(textround.c_str()));

	_textNumber->setString(historyStruct.prizeNumber);

	if (historyStruct.result == "long")
	{
		_spType->setTexture("numberRes/number_dragon.png");
	}
	else if (historyStruct.result == "hu")
	{
		_spType->setTexture("numberRes/number_tiger.png");
	}
	else if (historyStruct.result == "he")
	{
		_spType->setTexture("numberRes/number_tie.png");
	}

	return true;
}
