#pragma once
#include "cocos2d.h"
#include "ui\CocosGUI.h"
#include "editor-support\cocostudio\CocoStudio.h"

class DiaLogLayer : public cocos2d::Layer
{
public:

	virtual bool init();
	void btnCallback(cocos2d::Ref *pSender);
	void setLog(std::string logger);
	void setBtnFunction(std::function <void()> btnFunction);
	void setConfirmBtnFunction(std::function <void()> btnFunction);

	void setButtonStatus(bool isVisible){ _btnYes->setVisible(isVisible); }
	void setIsTwoButton(bool isTwo);
	void setAllButtonHide();

	CREATE_FUNC(DiaLogLayer);

private:

	std::function <void()> _btnFunction;
	std::function <void()> _confirmbtnFunction;

	cocos2d::ui::Button *_btnYes;
	cocos2d::ui::Button *_btnNo;
	cocos2d::ui::Button *_btnConfirm;

	cocos2d::ui::Text *_textLabel;
};