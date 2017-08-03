#include "AW_DiaLogLayer.h"
#include "AW_LoginScene.h"
#include "AW_GameManager.h"

USING_NS_CC;

bool DiaLogLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto diaLogNode = CSLoader::createNode("dialogLayer.csb");
	this->addChild(diaLogNode);

	_btnYes = diaLogNode->getChildByName<ui::Button *>("Button_Yes");
	_btnNo = diaLogNode->getChildByName<ui::Button *>("Button_No");
	_btnConfirm = diaLogNode->getChildByName<ui::Button *>("Button_confirm");
	_textLabel = diaLogNode->getChildByName<ui::Text *>("Text_label");

	_btnYes->addClickEventListener(CC_CALLBACK_1(DiaLogLayer::btnCallback,this));
	_btnNo->addClickEventListener(CC_CALLBACK_1(DiaLogLayer::btnCallback, this));
	_btnConfirm->addClickEventListener(CC_CALLBACK_1(DiaLogLayer::btnCallback, this));
	Director::getInstance()->getRunningScene()->addChild(this,10000);

	return true;
}

void DiaLogLayer::setLog(std::string logger)
{
	_textLabel->setString(logger);
}

void DiaLogLayer::setIsTwoButton(bool isTwo)
{
	if (isTwo)
	{
		_btnYes->setVisible(true);
		_btnNo->setVisible(true);
		_btnConfirm->setVisible(false);
	}
	else
	{
		_btnYes->setVisible(false);
		_btnNo->setVisible(false);
		_btnConfirm->setVisible(true);
	}
}

void DiaLogLayer::setAllButtonHide()
{
	_btnYes->setVisible(false);
	_btnNo->setVisible(false);
	_btnConfirm->setVisible(false);
}

void DiaLogLayer::setBtnFunction(std::function <void()> btnFunction)
{
	if (_btnFunction == nullptr)
	{
		_btnFunction = btnFunction;
	}
}

void DiaLogLayer::setConfirmBtnFunction(std::function <void()> btnFunction)
{
	if (_confirmbtnFunction == nullptr)
	{
		_confirmbtnFunction = btnFunction;
	}
}

void DiaLogLayer::btnCallback(cocos2d::Ref *pSender)
{
	if (pSender == _btnYes)
	{
		if (_btnFunction != nullptr)
		{
			_btnFunction();
			this->removeFromParent();
		}
		else
		{
			this->removeFromParent();
		}
	}
	else if (_btnNo == pSender)
	{
		this->removeFromParent();
	}
	else if (_btnConfirm == pSender)
	{
		if (_confirmbtnFunction != nullptr)
		{
			_confirmbtnFunction();
			this->removeFromParent();
		}
		else
		{
			this->removeFromParent();
		}
	}
}