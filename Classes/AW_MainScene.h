#pragma once

#include "AW_MessageHead.h"
#include "network/HttpClient.h"

using namespace cocos2d::network;

class TapInLayer;
class GameTimer;
class BoardCast;
class CCPomeloRequestResult;
class LoadingLayer;

class MainScene : public Layer
{
public:

	static Scene *createScene();

	virtual bool init();
	virtual void onEnter();

	MainScene();
	~MainScene();
	CREATE_FUNC(MainScene);

	void logicBtnCallback(Ref *pSender);
	void btnCallback(Ref *pSender, Widget::TouchEventType type);
	void historyBtnCallback(Ref *pSender);
	void hisNumberBtnCallback(Ref * pSender);

	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* event1);

	void setLotteryResult(LotteryResultSt lottery);
	void setBetResult(BetStruct betStruct);
	void setOdds(Odds odds);
	void displayResult(HistoryStruct his);
	void saveMenuList();
	void addCardToList();

	void pushNewItem(HistoryStruct historyStruct);

	void refreshCoin();
	void refreshCommission();
	void refreshAllBetInfo();

	void updateTimer(float dt, TIME_TYPE type);
    virtual void update(float delta);
    
	std::string getCountString();

	
	///ÏÂÔØ¶þÎ¬ÂëÍ¼Æ¬
	void downloadPic();
	void onDownloadComplete(HttpClient *sender, HttpResponse *response);

	void getLotteryInfoCallback(const CCPomeloRequestResult& result);
	void getHisNumberCallback(const CCPomeloRequestResult& result);

private:

	Sprite *_spBetIconDra;
	Sprite *_spBetIconTie;
	Sprite *_spBetIconTiger;

	//node hisNum
	Node *_nodeHisNum;
	Button *_btnHisNumClose;
	ListView *_lvHisNum;
	///

	Sprite *_spWin;

	ParticleSystem *_particleWin;
	ParticleSystem *_particleCircle;

	Button *_btnSave;
	Button*_btnGet;
	Button *_btnTwo;
	Button *_btnHistory;
	Button *_btnLogout;

	Button *_btnTie;
	Button *_btnTiger;
	Button *_btnDragon;

	Node *_nodeHistory;
	PageView *_historyPageView;
	Button *_btnHistoryUp;
	Button *_btnHistoryDown;
	Button *_btnHistoryEndUp;
	Button *_btnHistoryEndDown;
	Button *_btnHistoryClose;
	Button *_btnList;

	Button *_btnSet;

	Layout *_layout;
	Layout *_layoutClone;
	Layout *_panelEr;
	Layout *_layoutWeb;

	Sprite *_imageEr;

	TapInLayer *_tapInLayer;
	Text *_textCount;
	Text *_textCurrent;
	Text *_textNext;
	Text *_textHistoryNumber;

	Text *_textTieMoney;
	Text *_textTigerMoney;
	Text *_textDragonMoney;
	Text *_textCoin;

	Text *_textOddTiger;
	Text *_textOddDragon;
	Text *_textOddTie;

	Text *_textWincoin;

	Text *_textCommission;
	Text *_textBetStop;

	cocostudio::Armature *_animatePrize;

	Sprite *_spNumber[5];
	Label *_textNumber[5];

	int asd = 0;
	int _countTime;
	int _hisItemIndex;
	int _lastItemPosY;
	int _pageNumberIndex;

	bool _isTapLayerMoving;
	bool _isMenuOut;

	GameTimer *_gameTimer;

	int _menuPosX[7];

	Layout *_layoutBankCard;
	ListView *_lvcard;

	cocos2d::Vec2 _getStartPos;
	cocos2d::Vec2 _getEndPos;

	BoardCast *_boardCast;
	LoadingLayer *_loadingLayer;
};
