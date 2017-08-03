#pragma once
#include "AW_MessageHead.h"

class CCPomeloRequestResult;
class SetLayer : public Layer
{
public:

	SetLayer();
	~SetLayer();
	CREATE_FUNC(SetLayer);
	virtual bool init();
	void btnCallback(Ref* pSender,Widget::TouchEventType type);
	void bindCardCallback(const CCPomeloRequestResult& result);
	void modifyCardCallback(const CCPomeloRequestResult& result);
	
	void SoundMusicCallback(Ref *pSender, CheckBox::EventType type);

	void deleteCard(std::string _id);
	virtual void onEnter();
	void modifyCard(CardStruct cardstruct);

private:

	PageView *_pageView;
	ListView* _lvCard;

	TextField *_tfCard;
	TextField *_tfName;
	TextField *_tfOpenBankName;
	TextField *_tfBankName;

	Button *_btnNext;
	Button *_btnBack;
	Button *_btnClose;

	CheckBox *_cbSound;
	CheckBox *_cbMusic;

	std::string _modifyId;
	bool _isModifyCard;
};