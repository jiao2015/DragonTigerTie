#include "AW_CardItem.h"
#include "AW_DiaLogLayer.h"
#include "PomeloWrapper.h"
#include "AW_GameManager.h"

CardItem::CardItem()
	:
	_textBank(nullptr)
	, _textCard(nullptr)
	, _textName(nullptr)
	, _layoutBg(nullptr)
	, _cardStruct()
	, timeInv(0)
	, _getStartPos(0,0)
	, _getEndPos(0,0)
{

}

bool CardItem::init()
{
	AW_INIT_IF(!ui::Layout::init());

	auto cardNode = CSLoader::createNode("CardItem.csb");
	addChild(cardNode, 10);

	_textBank = cardNode->getChildByName<Text *>("Text_bank");
	_textCard = cardNode->getChildByName<Text *>("Text_number");
	 _textName = cardNode->getChildByName<Text *>("Text_name");
	 _layoutBg = cardNode->getChildByName<Layout *>("Panel_bg");

	 this->setContentSize(_layoutBg->getContentSize());

	 this->addTouchEventListener(CC_CALLBACK_2(CardItem::touchEventCallback, this));

	return true;
}

void CardItem::setProfile(CardStruct cardStruct)
{
	_cardStruct = cardStruct;

	_textBank->setString(cardStruct.cardBank);
	_textCard->setString(cardStruct.cardNumber);
	_textName->setString(cardStruct.userName);
}

void CardItem::touchEventCallback(Ref *pSender, Widget::TouchEventType type)
{
	ui::Layout *btn = static_cast<ui::Layout *>(pSender);

	if (Widget::TouchEventType::BEGAN == type)
	{
		_getStartPos = btn->getTouchBeganPosition();

		if (!isScheduled("asdf"))
		{
			schedule([this](float dt){
				timeInv++;
				if (timeInv == 2)
				{
					unschedule("asdf");
				}
			}, 1.0f, "asdf");
		}
	}
	else if (Widget::TouchEventType::ENDED == type || Widget::TouchEventType::CANCELED == type)
	{
		if (timeInv == 2)
		{
			timeInv = 0;
			auto dialog = DiaLogLayer::create();
			dialog->setLog(G2U("确定要删除此卡吗？"));
			//dialog->setButtonStatus(true);
			dialog->setIsTwoButton(true);
			dialog->setBtnFunction([this](){
				rapidjson::Document document4;
				document4.SetObject();
				rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
				document4.AddMember("id", rapidjson::Value(_cardStruct._id.c_str(), allocator4), allocator4);

				rapidjson::StringBuffer buffer4;
				rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
				document4.Accept(bb4);

				DisPatcher->dispatchCustomEvent("LoadingAddIndex");
				PomeloInstance->request("game.gameHandler.removeBankCard", buffer4.GetString(), CC_CALLBACK_1(CardItem::deleteCardCallback, this));

			});
		}
		else
		{
			unschedule("asdf");

			_getEndPos = btn->getTouchEndPosition();

            int between = std::abs(_getEndPos.y - _getStartPos.y);

			//CCLOG("between = %d", between);

			if (between >= 10)
			{
				return;
			}

			DisPatcher->dispatchCustomEvent("ShowPageChange", &_cardStruct);
			timeInv = 0;
		}
	}
}

void CardItem::deleteCardCallback(const CCPomeloRequestResult& result)
{
	CCLOG("CardItem-deleteCardCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		std::string _id = _cardStruct._id;
		DisPatcher->dispatchCustomEvent("DeleteCard", &_id);
	}

	DisPatcher->dispatchCustomEvent("LoadingMinIndex");

}
