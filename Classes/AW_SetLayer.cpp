#include "AW_SetLayer.h"
#include "AW_CardItem.h"
#include "AW_DiaLogLayer.h"
#include "PomeloWrapper.h"
#include "AW_GameManager.h"
#include "AW_SoundManger.h"

SetLayer::SetLayer()
	:
	_pageView(nullptr)
	, _lvCard(nullptr)
	,_tfCard(nullptr)
	,_tfName(nullptr)
	,_tfOpenBankName(nullptr)
	,_btnNext(nullptr)
	,_btnBack(nullptr)
	, _btnClose(nullptr)
	, _tfBankName(nullptr)
	, _isModifyCard(false)
	, _modifyId("")
	, _cbSound(nullptr)
	,_cbMusic(nullptr)

{

}

SetLayer::~SetLayer()
{
	DisPatcher->removeCustomEventListeners("DeleteCard");
	DisPatcher->removeCustomEventListeners("ShowPageChange");
}

void SetLayer::onEnter()
{
	Layer::onEnter();

	DisPatcher->addCustomEventListener("DeleteCard", [this](EventCustom *custom)
	{
		std::string *_id = static_cast<std::string *>(custom->getUserData());
		deleteCard(*_id);
	});

	DisPatcher->addCustomEventListener("ShowPageChange", [this](EventCustom *custom)
	{
		_pageView->scrollToPage(1, SetLayerMoveTime);
		CardStruct *cardstruct = static_cast<CardStruct *>(custom->getUserData());
		modifyCard(*cardstruct);
		_modifyId = (*cardstruct)._id;
		_isModifyCard = true;
	});
}

bool SetLayer::init()
{
	AW_INIT_IF(!Layer::init());

	auto setNode = CSLoader::createNode("SetLayer.csb");
	addChild(setNode, 10);

	_pageView = setNode->getChildByName<PageView *>("PageView");
	auto panel_1 = static_cast<Layout *>(_pageView->getChildByName("Panel_1"));
	auto panel_2 = static_cast<Layout *>(_pageView->getChildByName("Panel_2"));

	_lvCard = panel_1->getChildByName<ListView *>("ListView_addCard");
	_tfCard = panel_2->getChildByName<TextField *>("TextField_card");
	_tfName = panel_2->getChildByName<TextField *>("TextField_name");
	_tfOpenBankName = panel_2->getChildByName<TextField *>("TextField_backOpenName");
	_btnNext = panel_2->getChildByName<Button *>("Button_next");
	_btnBack = panel_2->getChildByName<Button *>("Button_back");
	_tfBankName = panel_2->getChildByName<TextField *>("TextField_backName");

	_btnClose = panel_1->getChildByName<Button *>("Button_close");
	_btnClose->addTouchEventListener(CC_CALLBACK_2(SetLayer::btnCallback, this));
	_btnBack->addTouchEventListener(CC_CALLBACK_2(SetLayer::btnCallback, this));
	_btnNext->addTouchEventListener(CC_CALLBACK_2(SetLayer::btnCallback, this));

	_cbSound = panel_1->getChildByName<CheckBox *>("CheckBox_Sound");
	_cbMusic = panel_1->getChildByName<CheckBox *>("CheckBox_Music");

	_cbSound->addEventListener(CC_CALLBACK_2(SetLayer::SoundMusicCallback, this));
	_cbMusic->addEventListener(CC_CALLBACK_2(SetLayer::SoundMusicCallback, this));

	if (GetLocalMusic)
	{
		_cbMusic->setSelected(false);
		_cbMusic->setBright(false);
	}
	else
	{
		_cbMusic->setSelected(true);
		_cbMusic->setBright(true);
	}

	if (GetLocalSound)
	{
		_cbSound->setSelected(false);
		_cbSound->setBright(false);
	}
	else
	{
		_cbSound->setSelected(true);
		_cbSound->setBright(true);
	}

	auto addPanel = static_cast<Layout *>( _lvCard->getChildByName("Panel_add"));
	addPanel->addClickEventListener([this](Ref *psender){
		_pageView->scrollToPage(1, SetLayerMoveTime);
		_tfCard->setString("");
		_tfName->setString("");
		_tfOpenBankName->setString("");
		_tfBankName->setString("");
	});

	for (int i = 0; i < GameInstance->_cardList.size(); i++)
	{
		auto cardItem = CardItem::create();
		cardItem->setTouchEnabled(true);
		cardItem->setName(GameInstance->_cardList.at(i)._id);
		cardItem->setProfile(GameInstance->_cardList.at(i));
		_lvCard->insertCustomItem(cardItem, 0);
	}
	
	return true;
}

void SetLayer::modifyCard(CardStruct cardstruct)
{
	_tfCard->setString(cardstruct.cardNumber);
	_tfName->setString(cardstruct.userName);
	_tfOpenBankName->setString(cardstruct.cardOpenBank);
	_tfBankName->setString(cardstruct.cardBank);
}

void SetLayer::deleteCard(std::string _id)
{
	_lvCard->removeChildByName(_id);
	GameInstance->removeCardItem(_id);
	_lvCard->setInnerContainerSize(Size(CardItemSize.width, CardItemSize.height * _lvCard->getChildrenCount()));
}

void SetLayer::btnCallback(Ref* pSender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::ENDED)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");

		if (_btnClose == pSender)
		{
			this->removeFromParent();
		}
		else if (_btnBack == pSender)
		{
			_isModifyCard = false;
			_pageView->scrollToPage(0, SetLayerMoveTime);
		}
		else if (_btnNext == pSender)
		{
			if (_tfBankName->getString() == "" || _tfCard->getString() == "" || 
				_tfName->getString() == "" || _tfOpenBankName->getString() == "")
			{
				std::string msg = "输入有误";
				DiaLogLayer::create()->setLog(G2U(msg.c_str()));

				return;
			}

			if (_isModifyCard)
			{
				rapidjson::Document document4;
				document4.SetObject();
				rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
				document4.AddMember("bankCardNumber", rapidjson::Value(_tfCard->getString().c_str(), allocator4), allocator4);
				document4.AddMember("bankName", rapidjson::Value(_tfBankName->getString().c_str(), allocator4), allocator4);
				document4.AddMember("bankDeposit", rapidjson::Value(_tfOpenBankName->getString().c_str(), allocator4), allocator4);
				document4.AddMember("cardHolder", rapidjson::Value(_tfName->getString().c_str(), allocator4), allocator4);
				document4.AddMember("id", rapidjson::Value(_modifyId.c_str(), allocator4), allocator4);
				rapidjson::StringBuffer buffer4;
				rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
				document4.Accept(bb4);

				DisPatcher->dispatchCustomEvent("LoadingAddIndex");
				PomeloInstance->request("game.gameHandler.modifyBankCard", buffer4.GetString(), CC_CALLBACK_1(SetLayer::modifyCardCallback, this));
			}
			else
			{
				rapidjson::Document document4;
				document4.SetObject();
				rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
				document4.AddMember("bankCardNumber", rapidjson::Value(_tfCard->getString().c_str(), allocator4), allocator4);
				document4.AddMember("bankName", rapidjson::Value(_tfBankName->getString().c_str(), allocator4), allocator4);
				document4.AddMember("bankDeposit", rapidjson::Value(_tfOpenBankName->getString().c_str(), allocator4), allocator4);
				document4.AddMember("cardHolder", rapidjson::Value(_tfName->getString().c_str(), allocator4), allocator4);

				rapidjson::StringBuffer buffer4;
				rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
				document4.Accept(bb4);

				DisPatcher->dispatchCustomEvent("LoadingAddIndex");
				PomeloInstance->request("game.gameHandler.bindBankCard", buffer4.GetString(), CC_CALLBACK_1(SetLayer::bindCardCallback, this));
			}
		}
	}
}

void SetLayer::modifyCardCallback(const CCPomeloRequestResult& result)
{
	CCLOG("SetLayer-modifyCardCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
	}
	else if (d["code"].GetInt() == 200)
	{
		deleteCard(d["card"]["_id"].GetString());

		std::string msg = "修改成功";
		auto dialog = DiaLogLayer::create();
		dialog->setLog(G2U(msg.c_str()));
		dialog->setConfirmBtnFunction([this](){
			_pageView->scrollToPage(0, SetLayerMoveTime);
		});
		CardStruct cardStruct = CardStruct(d["card"]["cardHolder"].GetString(), d["card"]["bankCardNumber"].GetString(),
			d["card"]["bankName"].GetString(), d["card"]["bankDeposit"].GetString(), d["card"]["_id"].GetString());
		GameInstance->_cardList.push_back(cardStruct);
		auto cardItem = CardItem::create();
		cardItem->setTouchEnabled(true);
		cardItem->setName(cardStruct._id);
		cardItem->setProfile(cardStruct);
		_lvCard->insertCustomItem(cardItem, 0);
	}

	_isModifyCard = false;

	DisPatcher->dispatchCustomEvent("LoadingMinIndex");
}

void SetLayer::SoundMusicCallback(Ref *pSender, CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED)
	{
		if (pSender == _cbMusic)
		{
			_cbMusic->setBright(true);
			SoundManger::getInstance()->setMusicStatus("", true);
			SetLocalMusic(false);
		}
		else
		{
			_cbSound->setBright(true);
			SoundManger::getInstance()->setSoundStatus(true);
			SetLocalSound(false);
		}
	}
	else
	{
		if (pSender == _cbMusic)
		{
			_cbMusic->setBright(false);
			SoundManger::getInstance()->setMusicStatus("", false);
			SetLocalMusic(true);
		}
		else
		{
			_cbSound->setBright(false);
			SoundManger::getInstance()->setSoundStatus(false);
			SetLocalSound(true);
		}
	}
}


void SetLayer::bindCardCallback(const CCPomeloRequestResult& result)
{
	CCLOG("SetLayer-bindCardCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		std::string msg = "绑定成功";
		auto dialog = DiaLogLayer::create();
		dialog->setLog(G2U(msg.c_str()));
		dialog->setConfirmBtnFunction([this](){
			_pageView->scrollToPage(0, SetLayerMoveTime);
		});
		CardStruct cardStruct = CardStruct(d["card"]["cardHolder"].GetString(), d["card"]["bankCardNumber"].GetString(),
			d["card"]["bankName"].GetString(), d["card"]["bankDeposit"].GetString(), d["card"]["_id"].GetString());
		GameInstance->_cardList.push_back(cardStruct);
		auto cardItem = CardItem::create();
		cardItem->setTouchEnabled(true);
		cardItem->setName(cardStruct._id);
		cardItem->setProfile(cardStruct);
		_lvCard->insertCustomItem(cardItem, 0);
		_lvCard->setInnerContainerSize(Size(cardItem->getContentSize().width, cardItem->getContentSize().height * _lvCard->getChildrenCount()));
	}

	DisPatcher->dispatchCustomEvent("LoadingMinIndex");
}
