#include "AW_MainScene.h"
#include "AW_TapInLayer.h"
#include "AW_GameTimer.h"
#include "AW_UnicodeGB2312.h"
#include "AW_HistoryItem.h"
#include "AW_GameManager.h"
#include "AW_LoginScene.h"
#include "AW_DiaLogLayer.h"
#include "AW_BoardCast.h"
#include "AW_SetLayer.h"
#include "AW_CardItem.h"
#include "SimpleAudioEngine.h"
#include "AW_HistoryNumItem.h"
#include "AW_LoadingLayer.h"
#include <time.h>

using namespace CocosDenshion;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
using namespace cocos2d::experimental::ui;
#endif

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "../proj.android/jni/hellocpp/CppJniJava.h"
#endif

Scene *MainScene::createScene()
{
	auto scene = Scene::create();
	auto main = MainScene::create();
	scene->addChild(main);
	return scene;
}

const int PageItemNumber = 6;
const float PageScrollTime = 0.2f;

void MainScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* event1)
{
	if (keycode == EventKeyboard::KeyCode::KEY_BACK)
	{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		showTipDialog();
#endif
	}
}

MainScene::MainScene()
	:_btnDragon(nullptr)
	, _btnSave(nullptr)
	,_btnGet(nullptr)
	,_btnTwo(nullptr)
	,_btnHistory(nullptr)
	,_btnTie(nullptr)
	,_btnTiger(nullptr)
	, _tapInLayer(nullptr)
	, _layout(nullptr)
	, _isTapLayerMoving(false)
	, _animatePrize(nullptr)
	, _gameTimer(nullptr)
	, _countTime(0)
	, _textCount(nullptr)
	, _textCurrent(nullptr)
	, _textNext(nullptr)
	, _nodeHistory(nullptr)
	, _historyPageView(nullptr)
	,_btnHistoryUp(nullptr)
	,_btnHistoryDown(nullptr)
	,_btnHistoryClose(nullptr)
	, _hisItemIndex(0)
	, _layoutClone(nullptr)
	, _lastItemPosY(0)
	, _textHistoryNumber(nullptr)
	, _btnLogout(nullptr)
	, _pageNumberIndex(0)
	, _textTieMoney(nullptr)
	,_textTigerMoney(nullptr)
	,_textDragonMoney(nullptr)
	, _textCoin(nullptr)
	, _textOddTiger(nullptr)
	,_textOddDragon(nullptr)
	,_textOddTie(nullptr)
	, _textWincoin(nullptr)
	, _spBetIconDra(nullptr)
	,_spBetIconTie(nullptr)
	,_spBetIconTiger(nullptr)
	, _spWin(nullptr)
	,_particleWin(nullptr)
	,_particleCircle(nullptr)
	, _btnList(nullptr)
	, _isMenuOut(false)
	, _btnHistoryEndUp(nullptr)
	,_btnHistoryEndDown(nullptr)
	, _panelEr(nullptr)
	, _imageEr(nullptr)
	, _textCommission(nullptr)
	, _btnSet(nullptr)
	, _layoutBankCard(nullptr)
	,_lvcard(nullptr)
	, _boardCast(nullptr)
	, _nodeHisNum(nullptr)
	,_btnHisNumClose(nullptr)
	,_lvHisNum(nullptr)
	, _loadingLayer(nullptr)
{
	for (int i = 0; i < sizeof(_spNumber) / sizeof(_spNumber[0]); i++)
	{
		_spNumber[i] = nullptr;
	}

	for (int i = 0; i < sizeof(_menuPosX) / sizeof(_menuPosX[0]); i++)
	{
		_menuPosX[i] = 0;
	}
}

MainScene::~MainScene()
{
	DisPatcher->removeCustomEventListeners("ShowResult");
	DisPatcher->removeCustomEventListeners("MoveTapDown");
	DisPatcher->removeCustomEventListeners("DisplayResult");
	DisPatcher->removeCustomEventListeners("RefreshCoin");
	DisPatcher->removeCustomEventListeners("ReceiveNotice");
	DisPatcher->removeCustomEventListeners("LoadingAddIndex");
	DisPatcher->removeCustomEventListeners("LoadingMinIndex");
}

void MainScene::onEnter()
{
	Layer::onEnter();

	DisPatcher->addCustomEventListener("ShowResult", [this](EventCustom *custom)
	{
		LotteryResultSt *lottery = static_cast<LotteryResultSt *>(custom->getUserData());
		setLotteryResult(*lottery);
		setOdds(GameInstance->_odds);
	});

	DisPatcher->addCustomEventListener("RefreshCoin", [this](EventCustom *custom)
	{
		refreshCoin();
	});

	DisPatcher->addCustomEventListener("LoadingAddIndex", [this](EventCustom *custom)
	{
		_loadingLayer->addIndex();
	});

	DisPatcher->addCustomEventListener("LoadingMinIndex", [this](EventCustom *custom)
	{
		_loadingLayer->removeLoading();
	});


	DisPatcher->addCustomEventListener("RefreshCountTime", [this](EventCustom *custom)
	{
		refreshAllBetInfo();
	});

	DisPatcher->addCustomEventListener("ReceiveNotice", [this](EventCustom *custom)
	{
		std::string *msg = static_cast<std::string *>(custom->getUserData());
		_boardCast->showBoardCast(*msg);
		_boardCast->start();
	});

	DisPatcher->addCustomEventListener("RefreshCommission", [this](EventCustom *custom)
	{
		refreshCommission();
	});

	DisPatcher->addCustomEventListener("ChangeOdds", [this](EventCustom *custom)
	{
		setOdds(GameInstance->_odds);
	});

	DisPatcher->addCustomEventListener("MoveTapDown", [this](EventCustom *custom){
		BetStruct *betStruct = static_cast<BetStruct *>(custom->getUserData());
		setBetResult(*betStruct);
		this->btnCallback(_layout,Widget::TouchEventType::ENDED);
	});

	DisPatcher->addCustomEventListener("DisplayResult", [this](EventCustom *custom){
		HistoryStruct *his = static_cast<HistoryStruct *>(custom->getUserData());
		HistoryStruct history = *his;

		scheduleOnce([this, history](float dt){
			displayResult(history);
		}, 5.0f, "showresult");
	});

	//注册接收服务器事件
	GameInstance->addServerListener();
}

void MainScene::refreshCoin()
{
	auto text = __String::createWithFormat("%.2f", GameInstance->_currCoin);
	_textCoin->setString(text->getCString());
}

void MainScene::refreshCommission()
{
	auto text = __String::createWithFormat("%.2f", GameInstance->_commissionCoin);
	_textCommission->setString(text->getCString());
}

void MainScene::refreshAllBetInfo()
{
	_loadingLayer->addIndex();
	rapidjson::Document document4;
	document4.SetObject();
	rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
	rapidjson::StringBuffer buffer4;
	rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
	document4.Accept(bb4);

	PomeloInstance->request("center.centerHandler.getLotteryInfo", buffer4.GetString(), CC_CALLBACK_1(MainScene::getLotteryInfoCallback, this));
}

void MainScene::getLotteryInfoCallback(const CCPomeloRequestResult& result)
{
	CCLOG("MainScene-getLotteryInfoCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		GameInstance->_lastLotteryResult.nextInterval = d["info"]["nextInterval"].GetInt();
		_countTime = GameInstance->_lastLotteryResult.nextInterval / 1000;
		_loadingLayer->removeLoading();
	}
	else
	{
		DiaLogLayer::create()->setLog(G2U(result.jsonMsg.c_str()));
		_loadingLayer->removeLoading();
	}
}

void MainScene::getHisNumberCallback(const CCPomeloRequestResult& result)
{
	CCLOG("MainScene-getHisNumberCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		if (d["history"].IsArray())
		{
			_lvHisNum->removeAllChildren();

			for (int i = 0; i < d["history"].Size(); i++)
			{
				HisNumStruct hisNumStruct;

				hisNumStruct.dateString = d["history"][i]["expect"].GetString();
				
				if (d["history"][i]["openCode"].IsArray())
				{
					std::string prize = "";

					for (int j = 0; j < d["history"][i]["openCode"].Size(); j++)
					{
						prize = prize + " " + d["history"][i]["openCode"][j].GetString();
					}
					hisNumStruct.prizeNumber = prize;

					int first = Value(d["history"][i]["openCode"][0].GetString()).asInt();
					int last = Value(d["history"][i]["openCode"][4].GetString()).asInt();
					if (first > last)
					{
						hisNumStruct.result = "long";
					}
					else if (first == last)
					{
						hisNumStruct.result = "he";
					}
					else
					{
						hisNumStruct.result = "hu";
					}
				}
				auto hisItem = HistoryNumItem::create(hisNumStruct);
				_lvHisNum->pushBackCustomItem(hisItem);
			}
		}

		_loadingLayer->removeLoading();
	}
	else
	{
		DiaLogLayer::create()->setLog(G2U(result.jsonMsg.c_str()));
		_loadingLayer->removeLoading();
	}
}

void MainScene::update(float delta)
{
    
}

bool MainScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
    
	_loadingLayer = LoadingLayer::create();
	_loadingLayer->setVisible(false);
	addChild(_loadingLayer, 10000);

	srand((unsigned int)time(0));
    
    //int index = (int)rand() % (RandomMusicIndex + 1);
    //CCLOG("musicIndex = %d " , index);
	int index = 1;

    GameInstance->_gameMusicIndex = index;
    
    std::string musicString = "Sound/bgm" + Value(index).asString() + ".mp3";
    
	SoundManger::getInstance()->playMusic(musicString.c_str(), false);

	downloadPic();

    schedule([this](float st){
        bool isMusicon = GetLocalMusic;
        bool isBackPlay = !SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
        
        if (isBackPlay && !isMusicon)
        {
            CCLOG("huan music");
            
            if (GameInstance->_gameMusicIndex < RandomMusicIndex)
            {
                GameInstance->_gameMusicIndex++;
                std::string musicString = "Sound/bgm" + Value(GameInstance->_gameMusicIndex).asString() + ".mp3";
                SoundManger::getInstance()->playMusic(musicString.c_str(), false);
            }
            else
            {
                GameInstance->_gameMusicIndex = 0;
                std::string musicString = "Sound/bgm" + Value(GameInstance->_gameMusicIndex).asString() + ".mp3";
                SoundManger::getInstance()->playMusic(musicString.c_str(), false);
            }
        }
    }, 1.0f, "changeMusic");
    
	PomeloInstance->setDisconnectedCallback([this](){
		auto dialog = DiaLogLayer::create();
		std::string setlog = "网络已断开，请重新连接";
		dialog->setLog(G2U(setlog.c_str()));
		dialog->setConfirmBtnFunction([this](){
			GameInstance->clear();
			_gameTimer->stopTime();
			_gameTimer->release();
			PomeloInstance->stop();
			Director::getInstance()->replaceScene(LoginScene::createScene());
		});
	});
	
	auto nodeMain = CSLoader::createNode("MainScene.csb");
	auto timeline = CSLoader::createTimeline("MainScene.csb");
	timeline->gotoFrameAndPlay(0, true);
	nodeMain->runAction(timeline);
	////
	_boardCast = BoardCast::create();
	_boardCast->setPosition(nodeMain->getChildByName<Node *>("Node_boardcast")->getPosition());
	this->addChild(_boardCast, 100);
	///
	_layoutBankCard = nodeMain->getChildByName<Layout *>("Panel_bankcard");
	_lvcard = _layoutBankCard->getChildByName<ListView *>("ListView_addCard");

	///
	_btnSet = nodeMain->getChildByName<Button *>("Button_set");
	_btnSet->addTouchEventListener(CC_CALLBACK_2(MainScene::btnCallback,this));

	_spWin = nodeMain->getChildByName<Sprite *>("Sprite_win");
	_particleWin = nodeMain->getChildByName<ParticleSystem *>("Particle_win");
	_particleCircle = nodeMain->getChildByName<ParticleSystem *>("Particle_circle");

	//_particleCircle->setVisible(true);
	_particleCircle->stop();
	_particleWin->stop();

	addChild(nodeMain);
	auto visibleSize = Director::getInstance()->getVisibleSize();

	_btnDragon = nodeMain->getChildByName<Button *>("Button_dragon");
	_btnSave = nodeMain->getChildByName<Button *>("Button_save");
	_btnGet = nodeMain->getChildByName<Button *>("Button_get");
	_btnTwo = nodeMain->getChildByName<Button *>("Button_Two");
	_btnHistory = nodeMain->getChildByName<Button *>("Button_hisytory");
	_btnTie = nodeMain->getChildByName<Button *>("Button_tie");
	_btnTiger = nodeMain->getChildByName<Button *>("Button_tiger");
	_btnLogout = nodeMain->getChildByName<Button *>("Button_logout");

	_btnList = nodeMain->getChildByName<Button *>("Button_menu");
	_btnList->addTouchEventListener(CC_CALLBACK_2(MainScene::btnCallback, this));

	saveMenuList();

	refreshAllBetInfo();

	_layout = nodeMain->getChildByName<Layout *>("Panel");
	_layout->addTouchEventListener(CC_CALLBACK_2(MainScene::btnCallback,this));

	_tapInLayer = TapInLayer::create();
	_tapInLayer->setPosition(0, -TapLayerMoveVec.y);
	this->addChild(_tapInLayer, 10);

	_btnDragon->addClickEventListener(CC_CALLBACK_1(MainScene::logicBtnCallback,this));
	_btnTie->addClickEventListener(CC_CALLBACK_1(MainScene::logicBtnCallback, this));
	_btnTiger->addClickEventListener(CC_CALLBACK_1(MainScene::logicBtnCallback, this));

	_btnGet->addTouchEventListener(CC_CALLBACK_2(MainScene::btnCallback, this));
	_btnTwo->addTouchEventListener(CC_CALLBACK_2(MainScene::btnCallback, this));
	_btnHistory->addTouchEventListener(CC_CALLBACK_2(MainScene::btnCallback, this));
	_btnSave->addTouchEventListener(CC_CALLBACK_2(MainScene::btnCallback, this));
	_btnLogout->addTouchEventListener(CC_CALLBACK_2(MainScene::btnCallback, this));

	_textCount = nodeMain->getChildByName<ui::Text *>("Text_count");
	_textCurrent = nodeMain->getChildByName<ui::Text *>("Text_current");
	_textNext = nodeMain->getChildByName<ui::Text *>("Text_next");

	_textNext->addClickEventListener(CC_CALLBACK_1(MainScene::hisNumberBtnCallback, this));

	_textTieMoney = nodeMain->getChildByName<ui::Text *>("Text_tieMoney");
	_textTigerMoney = nodeMain->getChildByName<ui::Text *>("Text_tigerMoney");
	_textDragonMoney = nodeMain->getChildByName<ui::Text *>("Text_dragonMoney");
	_textCoin = nodeMain->getChildByName<ui::Text *>("Text_coin");

	_textCommission = nodeMain->getChildByName<ui::Text*>("Text_commission");
	refreshCommission();
	_textCommission->addTouchEventListener(CC_CALLBACK_2(MainScene::btnCallback,this));

	_spBetIconDra = nodeMain->getChildByName<Sprite *>("coinIcon_dragon");
	_spBetIconTiger = nodeMain->getChildByName<Sprite *>("coinIcon_tiger");
	_spBetIconTie = nodeMain->getChildByName<Sprite *>("coinIcon_tie");

	_textWincoin = nodeMain->getChildByName<ui::Text *>("Text_win");

	_panelEr = nodeMain->getChildByName<ui::Layout *>("Panel_erweima");
	_imageEr = _panelEr->getChildByName<Sprite *>("Image_erweima");

	_panelEr->addTouchEventListener(CC_CALLBACK_2(MainScene::btnCallback, this));

	(GameInstance->_lastBetList.dragonOdd == 0) ? _textDragonMoney->setString("") : _textDragonMoney->setString(__String::createWithFormat("%.2f", GameInstance->_lastBetList.dragonOdd)->getCString());
	(GameInstance->_lastBetList.tigerOdd == 0) ? _textTigerMoney->setString("") : _textTigerMoney->setString(__String::createWithFormat("%.2f", GameInstance->_lastBetList.tigerOdd)->getCString());
	(GameInstance->_lastBetList.tieOdd == 0) ? _textTieMoney->setString("") : _textTieMoney->setString(__String::createWithFormat("%.2f", GameInstance->_lastBetList.tieOdd)->getCString());

	GameInstance->_lastBetList.dragonOdd == 0 ? GameInstance->_isBetCount : GameInstance->_isBetCount = true;
	GameInstance->_lastBetList.tigerOdd == 0 ? GameInstance->_isBetCount : GameInstance->_isBetCount = true;
	GameInstance->_lastBetList.tieOdd == 0 ? GameInstance->_isBetCount : GameInstance->_isBetCount = true;

	GameInstance->_lastBetList.dragonOdd == 0 ? _spBetIconDra->setVisible(false) : _spBetIconDra->setVisible(true);
	GameInstance->_lastBetList.tigerOdd == 0 ? _spBetIconTiger->setVisible(false) : _spBetIconTiger->setVisible(true);
	GameInstance->_lastBetList.tieOdd == 0 ? _spBetIconTie->setVisible(false) : _spBetIconTie->setVisible(true);

	_textOddTiger = nodeMain->getChildByName<ui::Text *>("Text_oddTiger");
	_textOddDragon = nodeMain->getChildByName<ui::Text *>("Text_oddDragon");
	_textOddTie = nodeMain->getChildByName<ui::Text *>("Text_oddTie");
	
	setOdds(GameInstance->_odds);

	_textBetStop = nodeMain->getChildByName<Text *>("Text_betStop");
	
	//_textCoin->setString(Value(GameInstance->_currCoin).asString());
	refreshCoin();

	std::string curString = "第 " + GameInstance->_lastLotteryResult.expectNumber + " 期" + "      总计 120 期";
	std::string nextString = "倒计时  第 " + GameInstance->_lastLotteryResult.nextExpect + " 期";

	_textNext->setString(G2U(curString.c_str()).c_str());
	_textCurrent->setString(G2U(nextString.c_str()).c_str());

	_countTime = GameInstance->_lastLotteryResult.nextInterval / 1000;

	if (_countTime < GameInstance->_gameSetStruct.betCloseTime)
	{
		GameInstance->_isLockBet = true;
		_textBetStop->setVisible(true);
	}
	
	_animatePrize = nodeMain->getChildByName<cocostudio::Armature *>("ArmatureNode_prize");

	for (int i = 0; i < sizeof(_spNumber) / sizeof(_spNumber[0]); i++)
	{
		std::string spString = "number0" + Value(i + 1).asString();
		_spNumber[i] = nodeMain->getChildByName<Sprite *>(spString);
		std::string textString[] = { "shuzihong.png", "shuzihei.png", "shuzihei.png", "shuzihei.png", "shuzihong.png" };
		_textNumber[i] = Label::createWithCharMap(textString[i], 53.2, 54, '0');
		_textNumber[i]->setPosition(_spNumber[i]->getContentSize() / 2);
		_spNumber[i]->addChild(_textNumber[i]);
		if (!GameInstance->_lastLotteryResult.openCode.size())
		{
			break;
		}

		_textNumber[i]->setString(GameInstance->_lastLotteryResult.openCode.at(i));
		//_spNumber[i]->setVisible(false);
	}
	
	_gameTimer = GameTimer::create(TIME_TYPE::TIME_0, 1);
	_gameTimer->retain();
	_gameTimer->addGameTimer(CC_CALLBACK_2(MainScene::updateTimer,this));

	//history Node;
	_nodeHistory = CSLoader::createNode("HistoryNode.csb");
	this->addChild(_nodeHistory, 2);
	_historyPageView = _nodeHistory->getChildByName<PageView *>("PageView_history");
	_historyPageView->setTouchEnabled(false);

	_textHistoryNumber = _nodeHistory->getChildByName<Text *>("Text_number");
	_textHistoryNumber->setString("1 / 1");
	_nodeHistory->setPosition(Vec2(visibleSize.width + _historyPageView->getContentSize().width / 2 + 50, visibleSize.height / 2));
	
	_btnHistoryUp = _nodeHistory->getChildByName<Button *>("Button_up");
	_btnHistoryDown = _nodeHistory->getChildByName<Button *>("Button_down");
	_btnHistoryClose = _nodeHistory->getChildByName<Button *>("Button_close");

	_btnHistoryEndUp = _nodeHistory->getChildByName<Button *>("Button_up_end");
	_btnHistoryEndDown = _nodeHistory->getChildByName<Button *>("Button_down_end");

	_btnHistoryClose->addClickEventListener(CC_CALLBACK_1(MainScene::historyBtnCallback, this));
	_btnHistoryUp->addClickEventListener(CC_CALLBACK_1(MainScene::historyBtnCallback, this));
	_btnHistoryDown->addClickEventListener(CC_CALLBACK_1(MainScene::historyBtnCallback, this));
	_btnHistoryEndUp->addClickEventListener(CC_CALLBACK_1(MainScene::historyBtnCallback, this));
	_btnHistoryEndDown->addClickEventListener(CC_CALLBACK_1(MainScene::historyBtnCallback, this));

	_layoutClone = _historyPageView->getPage(0);

	_layoutWeb = nodeMain->getChildByName<Layout *>("Panel_web");
	_layoutWeb->setVisible(false);

	_lastItemPosY = _layoutClone->getContentSize().height;

	for (auto it = GameInstance->_historyList.begin(); it != GameInstance->_historyList.end(); it++)
	{
		pushNewItem((*it));
	}

	auto KeyListener = EventListenerKeyboard::create();
	KeyListener->onKeyReleased = CC_CALLBACK_2(MainScene::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(KeyListener, this);

	// node hisNumberItem
	_nodeHisNum = CSLoader::createNode("hisNumNode.csb");
	this->addChild(_nodeHisNum,2);
	_lvHisNum = _nodeHisNum->getChildByName<ListView *>("ListView");

	_nodeHisNum->setPosition(Vec2(-Director::getInstance()->getVisibleSize().width / 2, visibleSize.height / 2));

	_btnHisNumClose = _nodeHisNum->getChildByName<Button *>("Button_close");
	_btnHisNumClose->addClickEventListener(CC_CALLBACK_1(MainScene::hisNumberBtnCallback, this));

	//for (int i = 0; i < 20; i++)
	//{
	//	auto his = HistoryNumItem::create(HisNumStruct());
	//	_lvHisNum->pushBackCustomItem(his);
	//}


	////
	//for (int i = 0; i < 29; i++)
	//{
	//	pushNewItem(HistoryStruct());
	//}
	
	//scheduleOnce([this,boardCast](float dt){
	//	
	//	boardCast->showBoardCast("改一下做个测试，改一下做个测试");

	//}, 3.0f, "test");

	//scheduleOnce([this, boardCast](float dt){

	//	boardCast->stop();

	//}, 7.0f, "test1");

	//scheduleOnce([this, boardCast](float dt){

	//	boardCast->start();

	//}, 10.0f, "test2");


	return true;
}

void MainScene::saveMenuList()
{
	_btnList->getPositionX();

	float menuPosx[7] = { _btnList->getPositionX(), _btnSave->getPositionX(), _btnGet->getPositionX(),
		_btnTwo->getPositionX(), _btnSet->getPositionX(), _btnList->getPositionY(),_btnLogout->getPositionX() };

	for (int i = 0; i < sizeof(_menuPosX) / sizeof(_menuPosX[0]); i++)
	{
		_menuPosX[i] = menuPosx[i];
	}

	_btnGet->setPositionX(_btnList->getPositionX());
	_btnSave->setPositionX(_btnList->getPositionX());
	_btnTwo->setPositionX(_btnList->getPositionX());
	_btnSet->setPositionX(_btnList->getPositionX());
	_btnLogout->setPositionX(_btnList->getPositionX());
}

void MainScene::displayResult(HistoryStruct his)
{
	SimpleAudioEngine::getInstance()->stopAllEffects();

	(his.resultCount == "0.00") ? (_textWincoin->setString("")) : _textWincoin->setString("+" + his.resultCount);

	his.resultCount == "0.00" ? _spWin->setVisible(false) : _spWin->setVisible(true);
	his.resultCount == "0.00" ? _particleWin->stop() : _particleWin->start();
	
	if (his.resultCount == "0.00")
	{
		if (_textTieMoney->getString() != "" || _textTigerMoney->getString() != "" || _textDragonMoney->getString() != "")
		{
			SoundManger::getInstance()->playSound("Sound/lose.mp3");
		}
	}
	else
	{
		SoundManger::getInstance()->playSound("Sound/win.mp3");
	}

	if (his.result == "0")
	{
		_particleCircle->setPosition(_btnDragon->getPosition());
	}
	else if (his.result == "1")
	{
		_particleCircle->setPosition(_btnTiger->getPosition());
	}
	else if (his.result == "2")
	{
		_particleCircle->setPosition(_btnTie->getPosition());
	}

	_particleCircle->start();
	
	//_spWin->setVisible(true);

	scheduleOnce([this](float dt){
		_textWincoin->setString("");
		refreshCoin();
		//_textCoin->setString(Value(GameInstance->_currCoin).asString());
		_textTieMoney->setString("");
		_textTigerMoney->setString("");
		_textDragonMoney->setString("");
		_spBetIconDra->setVisible(false);
		_spBetIconTiger->setVisible(false);
		_spBetIconTie->setVisible(false);
		_spWin->setVisible(false);
		_textBetStop->setVisible(false);

		_particleCircle->stop();
		_particleWin->stop();

		GameInstance->_isLockBet = false;
		GameInstance->_isBetCount = false;
	}, 2.0f, "reSetText");
	pushNewItem(his);
}

void MainScene::hisNumberBtnCallback(Ref * pSender)
{
	if (_textNext == pSender)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		_boardCast->stop();
		_nodeHisNum->runAction(MoveTo::create(0.3f,
			Vec2(_lvHisNum->getContentSize().width / 2, Director::getInstance()->getVisibleSize().height / 2)));

		rapidjson::Document document4;
		document4.SetObject();
		rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
		rapidjson::StringBuffer buffer4;
		rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
		document4.Accept(bb4);
		_loadingLayer->addIndex();
		PomeloInstance->request("center.centerHandler.getLotteryHistory", buffer4.GetString(), CC_CALLBACK_1(MainScene::getHisNumberCallback, this));

	}
	else if (_btnHisNumClose == pSender)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		_boardCast->start();
		_nodeHisNum->runAction(MoveTo::create(0.3f,
			Vec2(-_lvHisNum->getContentSize().width / 2 - 50, Director::getInstance()->getVisibleSize().height / 2)));
	}
}

void MainScene::pushNewItem(HistoryStruct historyStruct)
{
	auto hisItem = HistoryItem::create(historyStruct);
	if (_hisItemIndex % PageItemNumber == 0 && _hisItemIndex != 0)
	{
		_lastItemPosY = _layoutClone->getContentSize().height;
		_historyPageView->addPage(_layoutClone->clone());
	}
	hisItem->setPosition(Vec2(0, _lastItemPosY));
	_historyPageView->getItem(_historyPageView->getChildrenCount() - 1)->addChild(hisItem);
	_lastItemPosY -= hisItem->getContentSize().height + 20;
	_hisItemIndex++;
}

void MainScene::historyBtnCallback(Ref *pSender)
{
	int pageCount = (int)_historyPageView->getChildrenCount();

	if (pSender == _btnHistoryClose)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		_boardCast->start();
		_nodeHistory->runAction(MoveTo::create(0.3f,
			Vec2(Director::getInstance()->getVisibleSize().width + _historyPageView->getContentSize().width / 2 + 50, Director::getInstance()->getVisibleSize().height / 2)));
	}
	else if (pSender == _btnHistoryUp)
	{
		if (_pageNumberIndex == 1)
		{
			return;
		}
		else
		{
			SoundManger::getInstance()->playSound("Sound/scrollPage.wav");
			_pageNumberIndex--;
			_textHistoryNumber->setString(Value(_pageNumberIndex).asString() + " / " + Value(pageCount).asString());
			_historyPageView->scrollToPage(_pageNumberIndex - 1, PageScrollTime);
		}
	}
	else if (pSender == _btnHistoryDown)
	{
		if (_pageNumberIndex == pageCount)
		{
			return;
		}
		else
		{
			SoundManger::getInstance()->playSound("Sound/scrollPage.wav");
			_pageNumberIndex++;
			_textHistoryNumber->setString(Value(_pageNumberIndex).asString() + " / " + Value(pageCount).asString());
			_historyPageView->scrollToPage(_pageNumberIndex - 1, PageScrollTime);
		}
	}
	else if (_btnHistoryEndDown == pSender)
	{
		if (_pageNumberIndex == pageCount)
		{
			return;
		}
		else
		{
			SoundManger::getInstance()->playSound("Sound/scrollPage.wav");
			_pageNumberIndex = pageCount;
			_textHistoryNumber->setString(Value(_pageNumberIndex).asString() + " / " + Value(pageCount).asString());
			_historyPageView->scrollToPage(_pageNumberIndex - 1, PageScrollTime);
		}
	}
	else if(_btnHistoryEndUp == pSender)
	{
		if (_pageNumberIndex == 1)
		{
			return;
		}
		else
		{
			SoundManger::getInstance()->playSound("Sound/scrollPage.wav");
			_pageNumberIndex = 1;
			_textHistoryNumber->setString(Value(_pageNumberIndex).asString() + " / " + Value(pageCount).asString());
			_historyPageView->scrollToPage(_pageNumberIndex - 1, PageScrollTime);
		}
	}
}

void MainScene::setOdds(Odds odds)
{
	std::string tigerodd = "1 赔 " + Value(__String::createWithFormat("%.2f", odds.tigerOdd)->getCString()).asString();
	std::string tieodd = "1 赔 " + Value(__String::createWithFormat("%.2f", odds.tieOdd)->getCString()).asString();
	std::string dragonodd = "1 赔 " + Value(__String::createWithFormat("%.2f", odds.dragonOdd)->getCString()).asString();
	_textOddTiger->setString(G2U(tigerodd.c_str()));
	_textOddTie->setString(G2U(tieodd.c_str()));
	_textOddDragon->setString(G2U(dragonodd.c_str()));
}

void MainScene::setBetResult(BetStruct betStruct)
{
	if (betStruct.betType == "numberRes/number_dragon.png")
	{
		_textDragonMoney->setString(betStruct.betCount);
		_spBetIconDra->setVisible(true);
	}
	else if (betStruct.betType == "numberRes/number_tiger.png")
	{
		_textTigerMoney->setString(betStruct.betCount);
		_spBetIconTiger->setVisible(true);
	}
	else if (betStruct.betType == "numberRes/number_tie.png")
	{
		_textTieMoney->setString(betStruct.betCount);
		_spBetIconTie->setVisible(true);
	}

	refreshCoin();
	//_textCoin->setString(Value(GameInstance->_currCoin).asString());
}

void MainScene::setLotteryResult(LotteryResultSt lottery)
{
	std::string curString = "第 " + lottery.expectNumber + " 期" + "      总计 120 期";
	std::string nextString = "倒计时  第 " + lottery.nextExpect + " 期";

	_countTime = lottery.nextInterval / 1000;

	SimpleAudioEngine::getInstance()->stopAllEffects();

	_textNext->setString(G2U(curString.c_str()));
	_textCurrent->setString(G2U(nextString.c_str()));

	_animatePrize->getAnimation()->play("kaijiang", -1, 1);
	for (int i = 0; i < 5; i++)
	{
		_spNumber[i]->setVisible(false);
	}

	asd = 0;

	schedule([this,lottery](float dt){
		if (asd >= sizeof(_spNumber) / sizeof(_spNumber[0]))
		{
			_animatePrize->getAnimation()->stop();
			unschedule("fresh");
			return;
		}
		_spNumber[asd]->setVisible(true);
		_textNumber[asd]->setString(lottery.openCode.at(asd));
		//SoundManger::getInstance()->playSound("Sound/confirmNumber.wav");
		asd++;
	}, 1.0f, "fresh");
}

std::string MainScene::getCountString()
{
	std::string hour = Value(_countTime / 3600).asString();
	std::string minute = Value((_countTime % 3600) / 60).asString();
	std::string second = Value((_countTime % 3600) % 60).asString();

	(hour.length() == 1) ? (hour = "0" + hour) : hour;
	(minute.length() == 1) ? (minute = "0" + minute) : minute;
	(second.length() == 1) ? (second = "0" + second) : second;

	return (hour + ":" + minute + ":" + second);
}

void MainScene::addCardToList()
{
	_lvcard->removeAllChildren();

	for (int i = 0; i < GameInstance->_cardList.size(); i++)
	{
		auto cardItem = CardItem::create();
		cardItem->setTouchEnabled(true);
		cardItem->addTouchEventListener([this](Ref *pSender,Widget::TouchEventType type){
			auto carditem = static_cast<CardItem *>(pSender);

			//取款
			if (_isTapLayerMoving)
			{
				return;
			}

			if (Widget::TouchEventType::BEGAN == type)
			{
				_getStartPos = carditem->getTouchBeganPosition();
			}
			else if (Widget::TouchEventType::ENDED == type || Widget::TouchEventType::CANCELED == type)
			{
				_getEndPos = carditem->getTouchEndPosition();

				int between = std::abs(_getEndPos.y - _getStartPos.y);

				//CCLOG("between = %d", between);

				if (between >= 10)
				{
					return;
				}

				_tapInLayer->setCardStruct(carditem->getCardStruct());
				_tapInLayer->setImageType("qu.png");
				_isTapLayerMoving = true;
				_layout->setVisible(true);
				_tapInLayer->runAction(Sequence::create(MoveTo::create(TapLayerMoveTime, Vec2(0, 0)), CallFunc::create([this](){
					_isTapLayerMoving = false;
				}), NULL));
				_layoutBankCard->setVisible(false);
				_lvcard->setPosition(Vec2(0, -200));
			}
		});
		cardItem->setName(GameInstance->_cardList.at(i)._id);
		cardItem->setProfile(GameInstance->_cardList.at(i));
		_lvcard->insertCustomItem(cardItem, 0);
	}
}

void MainScene::updateTimer(float dt, TIME_TYPE type)
{
	if (_countTime == -1)
	{
		return;
	}

	if (_countTime == GameInstance->_gameSetStruct.betCloseTime)
	{
		GameInstance->_isLockBet = true;
		_textBetStop->setVisible(true);
		SoundManger::getInstance()->playSound("Sound/confirmNumber.wav");

		this->btnCallback(_layout,Widget::TouchEventType::ENDED);
		//std::string logstring = "押注结束，请等待下一轮押注";
		//auto dialog = DiaLogLayer::create();
		//dialog->setLog(G2U(logstring.c_str()));
		//dialog->setAllButtonHide();
		//scheduleOnce([this, dialog](float dt){
		//	dialog->removeFromParent();
		//}, 2.0f, "dialog");
	}
	else if (_countTime <= StartCountTime)
	{
		SoundManger::getInstance()->playSound("Sound/confirmNumber.wav");
	}

	_textCount->setString(getCountString());
	_countTime--;

	if (_countTime == -1)
	{
		SimpleAudioEngine::getInstance()->stopAllEffects();
	}
}

void MainScene::logicBtnCallback(Ref *pSender)
{
	if (GameInstance->_isLockBet || _isTapLayerMoving)
	{
		return;
	}

	SoundManger::getInstance()->playSound("Sound/click.wav");

	if (pSender == _btnDragon)
	{
		_tapInLayer->setImageType("number_dragon.png");
	}
	else if (pSender == _btnTiger)
	{
		_tapInLayer->setImageType("number_tiger.png");
	}
	else if (pSender == _btnTie)
	{
		_tapInLayer->setImageType("number_tie.png");
	}
	
	_isTapLayerMoving = true;
	_layout->setVisible(true);
	_tapInLayer->runAction(Sequence::create(MoveTo::create(TapLayerMoveTime, Vec2(0, 0)), CallFunc::create([this](){
		_isTapLayerMoving = false;
	}), NULL));
}

void MainScene::btnCallback(Ref *pSender, Widget::TouchEventType type)
{
	if (Widget::TouchEventType::BEGAN == type || Widget::TouchEventType::CANCELED == type || Widget::TouchEventType::MOVED == type)
	{
		return;
	}

	if (pSender == _btnSave)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");

		_layoutWeb->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		auto winSize = Director::getInstance()->getWinSize();
		auto webView = WebView::create();
		webView->setAnchorPoint(Vec2(0, 0));
		webView->setPosition(Vec2(0,0));
		//webView->setScaleY(0.9f);
		webView->setContentSize(Size(winSize.width,winSize.height * 0.95f));
		//webView->setContentSize(winSize);
		webView->loadURL("http://www.baidu.com");
		webView->setScalesPageToFit(true);
		webView->setOnDidFailLoading([](WebView *sender, const std::string &url){
			CCLOG("failed");
		});
		this->addChild(webView, 100);

		Button *btn = Button::create("mainRes/closeWeb.png", "mainRes/closeWebT.png");
		btn->setAnchorPoint(Vec2(1, 0));
		btn->setPosition(Size(webView->getContentSize().width, webView->getContentSize().height));
		btn->addClickEventListener([this, webView, btn](Ref *pSender){
			webView->removeFromParent();
			btn->removeFromParent();
			_layoutWeb->setVisible(false);
		});

		this->addChild(btn, 101);

#endif
		//auto winSize = Director::getInstance()->getWinSize();
		//auto webView = Layout::create();
		//webView->setAnchorPoint(Vec2(0, 0));
		//webView->setContentSize(Size(winSize.width,winSize.height * 0.9f));
		
	}
	else if (pSender == _btnGet)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		if (GameInstance->_cardList.size() == 0)
		{
			auto dialog = DiaLogLayer::create();
			dialog->setIsTwoButton(false);
			dialog->setLog(G2U("请先到设置界面中绑定银行卡"));
		}
		else
		{
			_layoutBankCard->setVisible(true);
			addCardToList();
			_lvcard->runAction(MoveTo::create(0.4f, Vec2(0, 600)));
		}
	}
	else if (pSender == _btnTwo)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		auto picPath = FileUtils::getInstance()->getWritablePath() + GameInstance->_playerInfo.uid + ".png";
		_imageEr->setTexture(picPath);
		_imageEr->setScale(2.0f);
		_panelEr->setVisible(true);
	}
	else if (pSender == _btnHistory)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		_boardCast->stop();
		_pageNumberIndex = (int)_historyPageView->getChildrenCount();
		_historyPageView->scrollToPage(_pageNumberIndex - 1, PageScrollTime);
		_textHistoryNumber->setString(Value(_pageNumberIndex).asString() + " / " + Value(_pageNumberIndex).asString());

		_nodeHistory->runAction(MoveTo::create(0.3f, 
			Vec2(Director::getInstance()->getVisibleSize().width - _historyPageView->getContentSize().width / 2, Director::getInstance()->getVisibleSize().height / 2)));
	}
	else if (_layout == pSender)
	{
		if (Widget::TouchEventType::ENDED == type)
		{
			if (_isTapLayerMoving)
			{
				return;
			}
			_isTapLayerMoving = true;

			_tapInLayer->runAction(Sequence::create(MoveTo::create(TapLayerMoveTime, -TapLayerMoveVec), CallFunc::create([this](){
				_layout->setVisible(false);
				_isTapLayerMoving = false;
				_tapInLayer->clearNumber();
			}), NULL));
		}
	}
	else if (_btnLogout == pSender)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		_gameTimer->stopTime();
		_gameTimer->release();
		PomeloInstance->stop();
		GameInstance->clear();
		SoundManger::getInstance()->pauseMusic();

		Director::getInstance()->replaceScene(LoginScene::createScene());
	}
	else if (_btnList == pSender)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		_btnList->setTouchEnabled(false);

		if (_isMenuOut)
		{
			_btnSave->runAction(MoveTo::create(0.2f, Vec2(_menuPosX[0], _menuPosX[5])));
			_btnGet->runAction(MoveTo::create(0.2f, Vec2(_menuPosX[0], _menuPosX[5])));
			_btnTwo->runAction(MoveTo::create(0.2f, Vec2(_menuPosX[0], _menuPosX[5])));
			_btnLogout->runAction(MoveTo::create(0.2f, Vec2(_menuPosX[0], _menuPosX[5])));
			_btnSet->runAction(Sequence::create(MoveTo::create(0.2f, Vec2(_menuPosX[0], _menuPosX[5])), CallFunc::create([this](){
				_btnSave->setVisible(false);
				_btnGet->setVisible(false);
				_btnTwo->setVisible(false);
				_btnSet->setVisible(false);
				_btnLogout->setVisible(false);

				_isMenuOut = false;
				_btnList->setTouchEnabled(true);
			}), NULL));
		}
		else
		{
			_btnSave->setVisible(true);
			_btnGet->setVisible(true);
			_btnTwo->setVisible(true);
			_btnSet->setVisible(true);
			_btnLogout->setVisible(true);

			_btnSave->runAction(MoveTo::create(0.2f, Vec2(_menuPosX[1], _menuPosX[5])));
			_btnGet->runAction(MoveTo::create(0.2f, Vec2(_menuPosX[2], _menuPosX[5])));
			_btnTwo->runAction(MoveTo::create(0.2f, Vec2(_menuPosX[3], _menuPosX[5])));
			_btnLogout->runAction(MoveTo::create(0.2f, Vec2(_menuPosX[6], _menuPosX[5])));
			_btnSet->runAction(Sequence::create(MoveTo::create(0.2f, Vec2(_menuPosX[4], _menuPosX[5])), CallFunc::create([this](){
				_isMenuOut = true;
				_btnList->setTouchEnabled(true);
			}), NULL));
		}
	}
	else if (_panelEr == pSender)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		_panelEr->setVisible(false);
	}
	else if (_textCommission == pSender)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		_tapInLayer->setImageType("number_commission.png");
		_isTapLayerMoving = true;
		_layout->setVisible(true);
		_tapInLayer->runAction(Sequence::create(MoveTo::create(TapLayerMoveTime, Vec2(0, 0)), CallFunc::create([this](){
			_isTapLayerMoving = false;
		}), NULL));
	}
	else if (_btnSet == pSender)
	{
		SoundManger::getInstance()->playSound("Sound/click.wav");
		auto setLayer = SetLayer::create();
		addChild(setLayer,102);
	}
}

void MainScene::downloadPic()
{
	HttpRequest *request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::GET);
	request->setTag("downLoad pic");
	std::string url = IP_DOWN_PNG + "/abc/create_qrcode?text=" + IP_DOWN_PNG + "/register/" + GameInstance->_playerInfo.uid;
	CCLOG("url = %s", url.c_str());
	request->setUrl(url);
	request->setResponseCallback(CC_CALLBACK_2(MainScene::onDownloadComplete, this));
	HttpClient::getInstance()->sendImmediate(request);
	request->release();
}

void MainScene::onDownloadComplete(HttpClient *sender, HttpResponse *response)
{
	if (!response) {
		std::string msg = "二维码获取失败，请联系管理员";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));
		return;
	}

	if (!response->isSucceed()) {
		std::string msg = "二维码获取失败，请联系管理员";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));

		CCLOG("error %s", response->getErrorBuffer());
		return;
	}

	CCLOG("get erweima");
	std::vector<char> *buffData = response->getResponseData();
	char *buff = (char *)malloc(buffData->size());
	std::copy(buffData->begin(), buffData->end(), buff);
	auto fileName = FileUtils::getInstance()->getWritablePath() + GameInstance->_playerInfo.uid + ".png";
	//CCLOG("path = %s", FileUtils::getInstance()->getWritablePath().c_str);

	FILE *fp = fopen(fileName.c_str(), "wb+");
	fwrite(buff, 1, buffData->size(), fp);
	fclose(fp);
}
