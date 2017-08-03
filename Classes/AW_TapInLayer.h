#pragma once
#include "AW_MessageHead.h"

class CCPomeloRequestResult;
class TapInLayer : public Node
{
public:

	static TapInLayer *create();
	TapInLayer();
	bool init();
	void btnCallback(Ref *pSender);

	void numberBtnCallback(Ref *pSender,Widget::TouchEventType type);
	void dotBtnCallback(Ref *pSender, Widget::TouchEventType type);

	void otherBtnCallback(Ref *pSender, Widget::TouchEventType type);
	void plusBtnCallback(Ref *pSender, Widget::TouchEventType type);

	void clearNumber();
	void setImageType(std::string type);
	void setCardStruct(CardStruct cardStruct){ _cardStruct = cardStruct; }

	void betCallback(const CCPomeloRequestResult& result);
	void getMoneyCallback(const CCPomeloRequestResult& result);
	void commission2CoinCallback(const CCPomeloRequestResult& result);

private:

	//static TapInLayer *_pInstance;
	bool _isText0;

	Button *_btnNumber[10];
	Button *_btnConfirm;
	Button *_btnDelete;
	Button *_btnDot;

	Button *_btnRetap;
	Button *_btn10;
	Button *_btn50;
	Button *_btn100;
	Button *_btn1000;
	Button *_btn10000;

	Text *_textShow;
	ImageView *_imageType;
	ImageView *_imageInnerType;

	std::string _betTypeString;
	std::string _betType;

	Layout *_layoutConfirm;
	Button *_btnConfirmTwo;
	Button *_btnCancel;
	Text *_textConfirm;
	CardStruct _cardStruct;
};