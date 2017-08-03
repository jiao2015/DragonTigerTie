#include "AW_HistoryItem.h"
#include "AW_UnicodeGB2312.h"

HistoryItem *HistoryItem::create(HistoryStruct historyStruct)
{
	auto history = new HistoryItem();
	if (history && history->init(historyStruct))
	{
		history->autorelease();
		return history;
	}
	return nullptr;
}

HistoryItem::HistoryItem()
	:
	_imageDragon(nullptr)
	,_imageTie(nullptr)
	,_imageTiger(nullptr)
	,_textRound(nullptr)
	,_textResult(nullptr)
	, _textDragon(nullptr)
	, _textTie(nullptr)
	,_textTiger(nullptr)
	,_textDragonPercent(nullptr)
	, _textTiePercent(nullptr)
	,_textTigerPercent(nullptr)
{

}

bool HistoryItem::init(HistoryStruct historyStruct)
{
	if (!Widget::init())
	{
		return false;
	}
	
	auto historyNode = CSLoader::createNode("historyItem.csb");
	this->setContentSize(historyNode->getContentSize());
	this->setAnchorPoint(Vec2(0, 1));
	this->addChild(historyNode);

	_imageDragon = historyNode->getChildByName<ImageView *>("Image_dragon");
	_imageTie = historyNode->getChildByName<ImageView *>("Image_tie");
	_imageTiger = historyNode->getChildByName<ImageView *>("Image_tiger");

	_textRound = historyNode->getChildByName<Text *>("Text_round");
	_textResult = historyNode->getChildByName<Text *>("Text_result");
	_textDragon = historyNode->getChildByName<Text *>("Text_dragon");
	_textTie = historyNode->getChildByName<Text *>("Text_tie");
	_textTiger = historyNode->getChildByName<Text *>("Text_tiger");
	_textDragonPercent = historyNode->getChildByName<Text *>("Text_dragon_percent");
	_textTiePercent = historyNode->getChildByName<Text *>("Text_tie_percent");
	_textTigerPercent = historyNode->getChildByName<Text *>("Text_tiger_percent");

	int result = Value(historyStruct.result).asInt();

	setWinImageOp(result);

	std::string textround = "第 " + historyStruct.dateString + " 期";

	_textRound->setString(G2U(textround.c_str()));

	int resultnumber = Value(historyStruct.resultCount).asInt() - Value(historyStruct.betDraCountString).asInt()
		- Value(historyStruct.betTieCountString).asInt() - Value(historyStruct.betTigCountString).asInt();
	std::string resultstring = "";
	resultnumber >= 0 ? (resultstring = "+" + Value(resultnumber).asString()) : (resultstring = Value(resultnumber).asString());
	resultnumber >= 0 ? (_textResult->setColor(Color3B::RED)) : (_textResult->setColor(Color3B::GREEN));

	resultnumber == 0 ? (_textResult->setString("0")) : (_textResult->setString(resultstring));

	_textDragon->setString(historyStruct.betDraCountString);
	_textTie->setString(historyStruct.betTieCountString);
	_textTiger->setString(historyStruct.betTigCountString);

	std::string betdra = "1 赔 " + historyStruct.betDraPercent;
	std::string bettie = "1 赔 " + historyStruct.betTiePercent;
	std::string bettig = "1 赔 " + historyStruct.betTigPercent;

	_textDragonPercent->setString(G2U(betdra.c_str()));
	_textTiePercent->setString(G2U(bettie.c_str()));
	_textTigerPercent->setString(G2U(bettig.c_str()));

	return true;
}

void HistoryItem::setWinImageOp(int result)
{
	//0 dragon 1 tiger 2 tie
	if (result == 0)
	{
		_imageDragon->setOpacity(255);
	}
	else if (result == 1)
	{
		_imageTiger->setOpacity(255);
	}
	else if (result == 2)
	{
		_imageTie->setOpacity(255);
	}
}
