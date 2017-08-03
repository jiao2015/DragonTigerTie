#include "AW_TapInLayer.h"
#include "AW_UnicodeGB2312.h"
#include "PomeloWrapper.h"
#include "AW_GameManager.h"
#include "AW_DiaLogLayer.h"

//TapInLayer *TapInLayer::_pInstance = nullptr;

TapInLayer *TapInLayer::create()
{
	auto tapin = new TapInLayer();

	if (tapin && tapin->init())
	{
		tapin->autorelease();
		return tapin;
	}

	CC_SAFE_DELETE(tapin);

	return nullptr;
}

TapInLayer::TapInLayer()
	:
	_btnConfirm(nullptr)
	, _btnDelete(nullptr)
	, _layoutConfirm(nullptr)
	, _textConfirm(nullptr)
	, _textShow(nullptr)
	, _btnConfirmTwo(nullptr)
	, _isText0(true)
	, _btnCancel(nullptr)
	, _betTypeString("")
	, _btnRetap(nullptr)
	,_btn10(nullptr)
	,_btn50(nullptr)
	,_btn100(nullptr)
	,_btn1000(nullptr)
	,_btn10000(nullptr)
	, _btnDot(nullptr)
	, _imageInnerType(nullptr)
	, _imageType(nullptr)
{
	for (int i = 0; i < sizeof(_btnNumber) / sizeof(_btnNumber[0]); i++)
	{
		_btnNumber[i] = nullptr;
	}
}

bool TapInLayer::init()
{
	if (!Node::init())
	{
		return false;
	}

	auto nodeTapIn = CSLoader::createNode("NumberLayer.csb");
	this->setContentSize(nodeTapIn->getContentSize());
	this->setAnchorPoint(Size(0, 0));
	addChild(nodeTapIn);

	_btnConfirm = nodeTapIn->getChildByName<Button *>("Button_confirm");
	_btnConfirm->addTouchEventListener(CC_CALLBACK_2(TapInLayer::otherBtnCallback, this));

	for (int i = 0; i < sizeof(_btnNumber) / sizeof(_btnNumber[0]); i++)
	{
		std::string numberString = "Button_" + Value(i).asString();
		_btnNumber[i] = nodeTapIn->getChildByName<Button *>(numberString);
		_btnNumber[i]->addTouchEventListener(CC_CALLBACK_2(TapInLayer::numberBtnCallback, this));
		_btnNumber[i]->setTag(i);
	}

	_textShow = nodeTapIn->getChildByName<Text *>("Text_show");
	_textShow->setString("0");

	_imageType = nodeTapIn->getChildByName<ImageView *>("Image_type");

	_btnDelete = nodeTapIn->getChildByName<Button *>("Button_delete");
	_btnDelete->addTouchEventListener(CC_CALLBACK_2(TapInLayer::otherBtnCallback, this));

	_layoutConfirm = nodeTapIn->getChildByName<Layout *>("Panel_confirm");
	_textConfirm = _layoutConfirm->getChildByName<Text *>("Text_confirm");
	_btnConfirmTwo = _layoutConfirm->getChildByName<Button *>("Button_confirmTwo");
	_imageInnerType = _layoutConfirm->getChildByName<ImageView *>("Image_inner_type");

	_btnConfirmTwo->addClickEventListener(CC_CALLBACK_1(TapInLayer::btnCallback, this));
	_btnCancel = _layoutConfirm->getChildByName<Button *>("Button_cancel");
	_btnCancel->addClickEventListener(CC_CALLBACK_1(TapInLayer::btnCallback, this));

	//
	_btnRetap = nodeTapIn->getChildByName<Button *>("Button_reTap");
	_btn10 = nodeTapIn->getChildByName<Button *>("Button_10");
	_btn50 = nodeTapIn->getChildByName<Button *>("Button_50");
	 _btn100 = nodeTapIn->getChildByName<Button *>("Button_100");
	 _btn1000 = nodeTapIn->getChildByName<Button *>("Button_1000");
	 _btn10000 = nodeTapIn->getChildByName<Button *>("Button_10000");
	 _btnDot = nodeTapIn->getChildByName<Button *>("Button_dot");

	 _btnDot->addTouchEventListener(CC_CALLBACK_2(TapInLayer::dotBtnCallback, this));
	 _btnRetap->addTouchEventListener(CC_CALLBACK_2(TapInLayer::otherBtnCallback, this));
	 _btn10->addTouchEventListener(CC_CALLBACK_2(TapInLayer::plusBtnCallback, this));
	 _btn50->addTouchEventListener(CC_CALLBACK_2(TapInLayer::plusBtnCallback, this));
	 _btn100->addTouchEventListener(CC_CALLBACK_2(TapInLayer::plusBtnCallback, this));
	 _btn1000->addTouchEventListener(CC_CALLBACK_2(TapInLayer::plusBtnCallback, this));
	 _btn10000->addTouchEventListener(CC_CALLBACK_2(TapInLayer::plusBtnCallback, this));
	 
	if (!nodeTapIn)
	{
		return false;
	}


	return true;
}

void TapInLayer::otherBtnCallback(Ref *pSender, Widget::TouchEventType type)
{
	Button *btn = static_cast<Button *>(pSender);

	if (type == Widget::TouchEventType::BEGAN)
	{
		btn->setOpacity(255);
		SoundManger::getInstance()->playSound("Sound/clicknumber.wav");
	}
	else if (type == Widget::TouchEventType::ENDED || type == Widget::TouchEventType::CANCELED)
	{
		btn->setOpacity(0);

		if (_btnConfirm == pSender)
		{
			_layoutConfirm->setVisible(true);
			std::string textString = "";
			if (_textShow->getString() == "0" || _textShow->getString()[_textShow->getString().length() - 1] == '.')
			{
				textString = "输入有误";
				_isText0 = true;
			}
			else
			{
				_isText0 = false;

				if (_betTypeString == "numberRes/number_commission.png")
				{
					if (Value(_textShow->getString()).asDouble() > GameInstance->_commissionCoin)
					{
						std::string msg = "佣金不足";
						DiaLogLayer::create()->setLog(G2U(msg.c_str()));
						_layoutConfirm->setVisible(false);
						return;
					}

					double text = Value(_textShow->getString()).asDouble();
					std::string text1 = __String::createWithFormat("%.2f",text)->getCString();

					textString = "您确认要提取" + text1 + "的佣金吗";
				}
				else if (_betTypeString == "numberRes/qu.png")
				{
					if ((Value(_textShow->getString()).asDouble() > GameInstance->_currCoin))
					{
						std::string msg = "金钱不足";
						DiaLogLayer::create()->setLog(G2U(msg.c_str()));
						_layoutConfirm->setVisible(false);
						return;
					}

					double text = Value(_textShow->getString()).asDouble();
					std::string text1 = __String::createWithFormat("%.2f", text)->getCString();

					textString = "您确认要取出" + text1 + "的金钱吗";
				}
				else
				{
					if ((Value(_textShow->getString()).asDouble() > GameInstance->_currCoin))
					{
						std::string msg = "金钱不足";
						DiaLogLayer::create()->setLog(G2U(msg.c_str()));
						_layoutConfirm->setVisible(false);
						return;
					}

					double text = Value(_textShow->getString()).asDouble();
					std::string text1 = String::createWithFormat("%.2f", text)->getCString();

					textString = "是否押 " + _betType + " "+ text1 + "？";
				}
				
			}
			_textConfirm->setString(G2U(textString.c_str()));
		}
		else if (_btnDelete == pSender)
		{
			std::string origonString = _textShow->getString();
			std::string newString = "";
			if (origonString.length() == 1)
			{
				newString = "0";
			}
			else
			{
				newString = origonString.substr(0, origonString.length() - 1);
			}

			_textShow->setString(newString);
		}
		else if (_btnRetap == pSender)
		{
			_textShow->setString("0");
		}
	}
}

void TapInLayer::plusBtnCallback(Ref *pSender, Widget::TouchEventType type)
{
	Button *btn = static_cast<Button *>(pSender);

	if (type == Widget::TouchEventType::BEGAN)
	{
		btn->setOpacity(255);
		SoundManger::getInstance()->playSound("Sound/clicknumber.wav");
	}
	else if (type == Widget::TouchEventType::ENDED || type == Widget::TouchEventType::CANCELED)
	{
		btn->setOpacity(0);

		int number = btn->getTag();

		double origonNumber = Value(_textShow->getString()).asDouble();
		std::string text1 = __String::createWithFormat("%.2f", origonNumber + number)->getCString();
		std::string newString = text1;
		_textShow->setString(newString);
	}
}

void TapInLayer::dotBtnCallback(Ref *pSender, Widget::TouchEventType type)
{
	Button *btn = static_cast<Button *>(pSender);

	if (type == Widget::TouchEventType::BEGAN)
	{
		btn->setOpacity(255);
		SoundManger::getInstance()->playSound("Sound/clicknumber.wav");
	}
	else if (type == Widget::TouchEventType::ENDED || type == Widget::TouchEventType::CANCELED)
	{
		btn->setOpacity(0);

		std::string origonString = _textShow->getString();

		bool idHaveDot = false;

		auto findIsHaveDot = origonString.find('.');
		if (findIsHaveDot != std::string::npos)
		{
			idHaveDot = true;
		}

		if (origonString[origonString.length() - 1] == '.' || idHaveDot)
		{
			return;
		}
		
		origonString += ".";
		_textShow->setString(origonString);
	}
}

void TapInLayer::numberBtnCallback(Ref *pSender, Widget::TouchEventType type)
{
	Button *btn = static_cast<Button *>(pSender);

	if (type == Widget::TouchEventType::BEGAN)
	{
		btn->setOpacity(255);
		SoundManger::getInstance()->playSound("Sound/clicknumber.wav");
	}
	else if (type == Widget::TouchEventType::ENDED || type == Widget::TouchEventType::CANCELED)
	{
		btn->setOpacity(0);
		int number = btn->getTag();
		std::string str = Value(number).asString();
		std::string origonString = _textShow->getString();
		if (str == "0" && origonString == "0")
		{
			return;
		}


		std::string newString = "";
		if (origonString == "0")
		{
			newString = str;
		}
		else
		{
			newString = origonString + str;
		}

		_textShow->setString(newString);
	}
}

void TapInLayer::btnCallback(Ref *pSender)
{
	SoundManger::getInstance()->playSound("Sound/clicknumber.wav");

	if (_btnConfirmTwo == pSender)
	{
		_layoutConfirm->setVisible(false);
		if (!_isText0)
		{
			if (_betTypeString == "numberRes/number_commission.png")
			{
				rapidjson::Document document4;
				document4.SetObject();
				rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
				document4.AddMember("num", Value(_textShow->getString()).asDouble(), allocator4);

				rapidjson::StringBuffer buffer4;
				rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
				document4.Accept(bb4);

				DisPatcher->dispatchCustomEvent("LoadingAddIndex");
				PomeloInstance->request("game.gameHandler.commission2coin", buffer4.GetString(), CC_CALLBACK_1(TapInLayer::commission2CoinCallback, this));

				BetStruct betStruct = BetStruct("", "");
				DisPatcher->dispatchCustomEvent("MoveTapDown", &betStruct);
			}
			else if (_betTypeString == "numberRes/qu.png")
			{
				rapidjson::Document document4;
				document4.SetObject();
				rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
				document4.AddMember("money", Value(_textShow->getString()).asDouble(), allocator4);
				document4.AddMember("bankCardNumber", rapidjson::Value(_cardStruct.cardNumber.c_str(), allocator4), allocator4);
				document4.AddMember("bankName", rapidjson::Value(_cardStruct.cardBank.c_str(), allocator4), allocator4);
				document4.AddMember("bankDeposit", rapidjson::Value(_cardStruct.cardOpenBank.c_str(), allocator4), allocator4);
				document4.AddMember("cardHolder", rapidjson::Value(_cardStruct.userName.c_str(), allocator4), allocator4);

				rapidjson::StringBuffer buffer4;
				rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
				document4.Accept(bb4);

				DisPatcher->dispatchCustomEvent("LoadingAddIndex");
				PomeloInstance->request("game.gameHandler.getMoney", buffer4.GetString(), CC_CALLBACK_1(TapInLayer::getMoneyCallback, this));

				BetStruct betStruct = BetStruct("", "");
				DisPatcher->dispatchCustomEvent("MoveTapDown", &betStruct);
			}
			else
			{
				if (GameInstance->_isLockBet)
				{
					BetStruct betStruct = BetStruct("", "");
					DisPatcher->dispatchCustomEvent("MoveTapDown", &betStruct);
					return;
				}
				
				double dragon = 0;
				double tiger = 0;
				double tie = 0;

				double betCount = Value(_textShow->getString()).asDouble();

				if (betCount < 0.01f)
				{
					DiaLogLayer::create()->setLog(G2U("最小投注不能小于0.01"));
					return;
				}

				if (_betTypeString == "numberRes/number_dragon.png")
				{
					dragon = betCount;
					dragon = Value(__String::createWithFormat("%.2f", dragon)->getCString()).asDouble();
				}
				else if (_betTypeString == "numberRes/number_tiger.png")
				{
					tiger = betCount;
					tiger = Value(__String::createWithFormat("%.2f", tiger)->getCString()).asDouble();
				}
				else if (_betTypeString == "numberRes/number_tie.png")
				{
					tie = betCount;
					tie = Value(__String::createWithFormat("%.2f", tie)->getCString()).asDouble();
				}

				rapidjson::Document document4;
				document4.SetObject();
				rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
				document4.AddMember("betDragon", dragon, allocator4);
				document4.AddMember("betTiger", tiger, allocator4);
				document4.AddMember("betEqual", tie, allocator4);

				rapidjson::StringBuffer buffer4;
				rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
				document4.Accept(bb4);

				DisPatcher->dispatchCustomEvent("LoadingAddIndex");
				PomeloInstance->request("game.gameHandler.bet", buffer4.GetString(), CC_CALLBACK_1(TapInLayer::betCallback, this));
			}
		}
	}
	else if (_btnCancel == pSender)
	{
		_layoutConfirm->setVisible(false);
	}
}

void TapInLayer::getMoneyCallback(const CCPomeloRequestResult& result)
{
	CCLOG("TapInLayer-getMoneyCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
	}
	else if (d["code"].GetInt() == 200)
	{
		DiaLogLayer::create()->setLog(G2U("已经提交申请，请等待审核"));
		GameInstance->_currCoin = d["coin"].GetDouble();
		DisPatcher->dispatchCustomEvent("LoadingMinIndex");
		DisPatcher->dispatchCustomEvent("RefreshCoin");
	}
	else
	{
		DiaLogLayer::create()->setLog(G2U(result.jsonMsg.c_str()));
		DisPatcher->dispatchCustomEvent("LoadingMinIndex");
	}
	
}

void TapInLayer::commission2CoinCallback(const CCPomeloRequestResult& result)
{
	CCLOG("TapInLayer-commission2CoinCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
	}
	else if (d["code"].GetInt() == 200)
	{
		d["commission"].IsDouble() ? GameInstance->_commissionCoin = d["commission"].GetDouble()
			: GameInstance->_commissionCoin = d["commission"].GetInt();
		
		d["coin"].IsDouble() ? GameInstance->_currCoin = d["coin"].GetDouble()
			: GameInstance->_currCoin = d["coin"].GetInt();

		DisPatcher->dispatchCustomEvent("LoadingMinIndex");
		DisPatcher->dispatchCustomEvent("RefreshCommission");
		DisPatcher->dispatchCustomEvent("RefreshCoin");
	}
	else if (d["code"].GetInt() == 501)
	{
		DisPatcher->dispatchCustomEvent("LoadingMinIndex");
		DiaLogLayer::create()->setLog(G2U("当月下注额不足，不能"));
	}
	else
	{
		DisPatcher->dispatchCustomEvent("LoadingMinIndex");
		DiaLogLayer::create()->setLog(G2U(result.jsonMsg.c_str()));
	}
}

void TapInLayer::betCallback(const CCPomeloRequestResult& result)
{
	CCLOG("TapInLayer-betCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
	}
	else if (d["code"].GetInt() == 200)
	{
		GameInstance->_currCoin = d["coin"].GetDouble();
		double betcount = 0;

		if (_betTypeString == "numberRes/number_dragon.png")
		{
			betcount = d["betInfo"]["betDragon"].GetDouble();
		}
		else if (_betTypeString == "numberRes/number_tiger.png")
		{
			betcount = d["betInfo"]["betTiger"].GetDouble();
		}
		else if (_betTypeString == "numberRes/number_tie.png")
		{
			betcount = d["betInfo"]["betEqual"].GetDouble();
		}

		//std::string betcount = _textShow->getString();
		BetStruct betStruct = BetStruct(__String::createWithFormat("%.2f", betcount)->getCString(), _betTypeString);

		GameInstance->_isBetCount = true;
		DisPatcher->dispatchCustomEvent("LoadingMinIndex");
		DisPatcher->dispatchCustomEvent("MoveTapDown", &betStruct);
	}
	else
	{
		DisPatcher->dispatchCustomEvent("LoadingMinIndex");
		DiaLogLayer::create()->setLog(G2U(result.jsonMsg.c_str()));
	}
}

void TapInLayer::clearNumber()
{
	_textShow->setString("0");
}

void TapInLayer::setImageType(std::string type)
{
	std::string imageString = "numberRes/" + type;
	_betTypeString = imageString;
	if (type == "number_dragon.png")
	{
		_betType = "龙";
	}
	else if (type == "number_tiger.png")
	{
		_betType = "虎";
	}
	else if (type == "number_tie.png")
	{
		_betType = "和";
	}

	_imageType->loadTexture(imageString);
	_imageInnerType->loadTexture(imageString);
}
