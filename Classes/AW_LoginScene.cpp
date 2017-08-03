#include "AW_LoginScene.h"
#include "AW_MainScene.h"
#include "AW_GameManager.h"
#include "AW_RegisterLayer.h"
#include "AW_DiaLogLayer.h"
#include "AW_UnicodeGB2312.h"
#include "AW_SoundManger.h"
#include "AW_LoadingLayer.h"

#include <math.h>
#include <iostream>
#include <iomanip>

Scene *LoginScene::createScene()
{
	auto scene = Scene::create();
	auto login = LoginScene::create();
	scene->addChild(login);
	return scene;
}

LoginScene::LoginScene()
	:
	_btnLogin(nullptr)
	, _btnRegister(nullptr)
	, _tfName(nullptr)
	, _tfPassWord(nullptr)
	, _btnEnglish(nullptr)
	,_btnWX(nullptr)
	,_btnForget(nullptr)
	, _loadingLayer(nullptr)
{

}
LoginScene::~LoginScene()
{
	DisPatcher->removeCustomEventListeners("RegisterCall");
}

bool LoginScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	_loadingLayer = LoadingLayer::create();
	_loadingLayer->setVisible(false);
	addChild(_loadingLayer,10000);

	DisPatcher->addCustomEventListener("RegisterCall", [this](EventCustom *custom)
	{
		auto registerInfo = static_cast<RegisterInfo *>(custom->getUserData());
		_registerInfo = *registerInfo;

		PomeloInstance->connectAsnyc(IP_NW.c_str(), 3114, CC_CALLBACK_1(LoginScene::registerConnectCallback, this));
		_loadingLayer->addIndex();
	});

	auto nodeLogin = CSLoader::createNode("LoginScene.csb");
	addChild(nodeLogin);

	_btnLogin = nodeLogin->getChildByName<Button *>("Button_login");
	_btnRegister = nodeLogin->getChildByName<Button *>("Button_register");
	_btnLogin->addClickEventListener(CC_CALLBACK_1(LoginScene::btnClickCallback, this));
	_btnRegister->addClickEventListener(CC_CALLBACK_1(LoginScene::btnClickCallback, this));

	std::string name = UserInstance->getStringForKey("username");
	std::string password = UserInstance->getStringForKey("password");

	_tfName = nodeLogin->getChildByName<TextField *>("TextField_name");
	_tfPassWord = nodeLogin->getChildByName<TextField *>("TextField_passWord");

	_tfName->setString(name);
	_tfPassWord->setString(password);

	if (GetLocalMusic)
	{
		SoundManger::getInstance()->setMusicStatus("", false);
	}
	else
	{
		SoundManger::getInstance()->setMusicStatus("", true);
	}

	if (GetLocalSound)
	{
		SoundManger::getInstance()->setSoundStatus(false);
	}
	else
	{
		SoundManger::getInstance()->setSoundStatus(true);
	}

	if (SoundManger::getInstance()->getCurrMusic() == "")
	{
		int index = (int)rand() % (RandomMusicIndex + 1);
		CCLOG("musicIndex = %d ", index);
		GameInstance->_gameMusicIndex = index;
		std::string musicString = "Sound/bgm" + Value(index).asString() + ".mp3";
		SoundManger::getInstance()->setCurrMusic(musicString);
	}
	return true;
}

void LoginScene::btnClickCallback(Ref * pSender)
{
	SoundManger::getInstance()->playSound("Sound/click.wav");

	if (pSender == _btnLogin)
	{
		//Director::getInstance()->replaceScene(MainScene::createScene());
		_loadingLayer->addIndex();
		PomeloInstance->connectAsnyc(IP_NW.c_str(), 3114, CC_CALLBACK_1(LoginScene::asynConnectCallback, this));
	}
	else if (pSender == _btnRegister)
	{
		auto registerLayer = RegisterLayer::create();
		addChild(registerLayer, 100);
	}
}

void LoginScene::registerConnectCallback(int result)
{
	if (result == 0)
	{
		auto userName = _registerInfo.username;
		auto passWord = _registerInfo.password;
		auto nickname = _registerInfo.nickname;

		rapidjson::Document document4;
		document4.SetObject();
		rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
		document4.AddMember("userName", rapidjson::Value(userName.c_str(), allocator4), allocator4);
		document4.AddMember("password", rapidjson::Value(passWord.c_str(), allocator4), allocator4);
		document4.AddMember("nickName", rapidjson::Value(nickname.c_str(), allocator4), allocator4);

		rapidjson::StringBuffer buffer4;
		rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
		document4.Accept(bb4);

		PomeloInstance->request("gate.loginHandler.register", buffer4.GetString(), CC_CALLBACK_1(LoginScene::queryEntryCallback, this));
	}
	else
	{
		std::string msg = "服务器连接错误";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));
		_loadingLayer->removeLoading();
	}
}

void LoginScene::asynConnectCallback(int result)
{
	if (result == 0)
	{
		auto userName = _tfName->getString();
		auto passWord = _tfPassWord->getString();

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		//CCLOG(G2U("必须没毛病啊").c_str());
#else
		CCLOG("必须没毛病啊");
#endif

		if (userName != "" && passWord != "")
		{
			rapidjson::Document document4;
			document4.SetObject();
			rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
			document4.AddMember("userName", rapidjson::Value(userName.c_str(), allocator4), allocator4);
			document4.AddMember("password", rapidjson::Value(passWord.c_str(), allocator4), allocator4);

			rapidjson::StringBuffer buffer4;
			rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
			document4.Accept(bb4);

			PomeloInstance->request("gate.loginHandler.login", buffer4.GetString(), CC_CALLBACK_1(LoginScene::queryEntryCallback, this));
		}
		else
		{
			DiaLogLayer::create()->setLog(G2U("账号或密码错误"));
			_loadingLayer->removeLoading();
		}
	}
	else
	{
		_loadingLayer->removeLoading();
		std::string msg = "服务器连接错误";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));
	}
}

void LoginScene::queryEntryCallback(const CCPomeloRequestResult& result)
{
	CCLOG("LoginScene-queryEntryCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		std::string ip = d["host"].GetString();
		int port = d["port"].GetInt();

		PomeloInstance->stop();
		if (PomeloInstance->connect(ip.c_str(), port) == 0)
		{
			
			GameInstance->_currCoin = d["playerInfo"]["coin"].GetDouble();
			GameInstance->_commissionCoin = d["playerInfo"]["commission"].GetDouble();

			GameInstance->_playerInfo = PlayerInfo(d["playerInfo"]["uid"].GetString(), d["playerInfo"]["nickName"].GetString()
				, d["playerInfo"]["icon"].GetString(), GameInstance->_currCoin);

			std::string verifyId = d["playerInfo"]["verifyId"].GetString();
			std::string uid = d["playerInfo"]["uid"].GetString();
			rapidjson::Document document4;
			document4.SetObject();
			rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
			document4.AddMember("verifyId", rapidjson::Value(verifyId.c_str(), allocator4), allocator4);
			document4.AddMember("uid", rapidjson::Value(uid.c_str(), allocator4), allocator4);

			rapidjson::StringBuffer buffer4;
			rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
			document4.Accept(bb4);

			PomeloInstance->request("connector.entryHandler.entry", buffer4.GetString(), CC_CALLBACK_1(LoginScene::connectorCallback, this));
		}
		else
		{
			std::string msg = "服务器连接错误";
			DiaLogLayer::create()->setLog(G2U(msg.c_str()));
			_loadingLayer->removeLoading();
			//CCLOG("error");
		}
	}
	else if (d["code"].GetInt() == 502)
	{
		_loadingLayer->removeLoading();
		std::string msg = "无此玩家";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));
	}
	else if (d["code"].GetInt() == 503)
	{
		_loadingLayer->removeLoading();
		std::string msg = "密码错误";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));
	}
	else if (d["code"].GetInt() == 504)
	{
		_loadingLayer->removeLoading();
		std::string msg = "服务器连接错误";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));
	}
	else
	{
		_loadingLayer->removeLoading();
		DiaLogLayer::create()->setLog(G2U(result.jsonMsg.c_str()));
	}
}

void LoginScene::connectorCallback(const CCPomeloRequestResult& result)
{
	CCLOG("LoginScene-connectorCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		auto userName = _tfName->getString();
		auto passWord = _tfPassWord->getString();

		auto originUser = UserInstance->getStringForKey("username");
		auto originPassWord = UserInstance->getStringForKey("password");

		if ((userName != originUser) && (passWord != originPassWord))
		{
			GameInstance->_historyList.clear();
		}

		UserInstance->setStringForKey("username", userName);
		UserInstance->setStringForKey("password", passWord);

		//在线人数
		GameInstance->_onlineCount = d["onlineAmount"].GetInt();
		
		std::string opencode = d["lotteryInfo"]["openCode"].GetString();
		std::string opentime = d["lotteryInfo"]["openTime"].GetString();
		std::string expectnumber = d["lotteryInfo"]["expect"].GetString();
		int nextInterval = d["lotteryInfo"]["nextInterval"].GetInt();
		std::string nextexpect = d["lotteryInfo"]["nextExpect"].GetString();

		GameInstance->_odds = Odds(d["lotteryInfo"]["odds"]["tiger"].GetDouble(), 
			d["lotteryInfo"]["odds"]["equal"].GetDouble(), d["lotteryInfo"]["odds"]["dragon"].GetDouble());

		std::vector<std::string> openCodeList;

		std::stringstream ss;
		for (int i = 0; i < 5; i++)
		{
			ss.str("");
			char ffas = opencode.at(i * 2);
			ss << ffas;
			std::string asfd = ss.str();
			openCodeList.push_back(asfd);
		}

		GameInstance->_lastLotteryResult = LotteryResultSt(expectnumber, opentime, openCodeList, nextInterval, nextexpect);

		rapidjson::Document document4;
		document4.SetObject();
		//rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
		rapidjson::StringBuffer buffer4;
		rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
		document4.Accept(bb4);

		PomeloInstance->request("game.gameHandler.getMyBetInfo", buffer4.GetString(), CC_CALLBACK_1(LoginScene::betCallback, this));
	}
	else if (d["code"].GetInt() == 502)
	{
		_loadingLayer->removeLoading();
		std::string msg = "无此玩家";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));
	}
	else if (d["code"].GetInt() == 503)
	{
		_loadingLayer->removeLoading();
		std::string msg = "验证id错误";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));
	}
	else if (d["code"].GetInt() == 504)
	{
		_loadingLayer->removeLoading();
		std::string msg = "玩家已登录";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));
	}
	else
	{
		_loadingLayer->removeLoading();
		DiaLogLayer::create()->setLog(G2U(result.jsonMsg.c_str()));
	}
}

void LoginScene::betCallback(const CCPomeloRequestResult& result)
{
	CCLOG("LoginScene-betCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		if (d.HasMember("betInfo"))
		{
			GameInstance->_lastBetList = Odds(d["betInfo"]["betTiger"].GetDouble(),
				d["betInfo"]["betEqual"].GetDouble(), d["betInfo"]["betDragon"].GetDouble());
		}
		
		rapidjson::Document document4;
		document4.SetObject();
		//rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
		rapidjson::StringBuffer buffer4;
		rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
		document4.Accept(bb4);

		PomeloInstance->request("game.gameHandler.getBindBankCards", buffer4.GetString(), CC_CALLBACK_1(LoginScene::getBankCardCallback, this));
	}
	else
	{
		_loadingLayer->removeLoading();
		std::string msg = "未知错误 ‘game.gameHandler.getMyBetInfo’";
		DiaLogLayer::create()->setLog(G2U(msg.c_str()));
	}
}

void LoginScene::getBankCardCallback(const CCPomeloRequestResult& result)
{
	CCLOG("LoginScene-getBankCardCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		if (d["cards"].IsArray())
		{
			GameInstance->_cardList.clear();

			for (int i = 0; i < d["cards"].Size(); i++)
			{
				CardStruct cardStruct = CardStruct(d["cards"][i]["cardHolder"].GetString(), d["cards"][i]["bankCardNumber"].GetString(),
					d["cards"][i]["bankName"].GetString(), d["cards"][i]["bankDeposit"].GetString(), d["cards"][i]["_id"].GetString());
				GameInstance->_cardList.push_back(cardStruct);
			}
		}

		rapidjson::Document document4;
		document4.SetObject();
		//rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
		rapidjson::StringBuffer buffer4;
		rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
		document4.Accept(bb4);

		PomeloInstance->request("game.gameHandler.getGameSetting", buffer4.GetString(), CC_CALLBACK_1(LoginScene::gameSettingCallback, this));
		
	}
	else
	{
		_loadingLayer->removeLoading();
		DiaLogLayer::create()->setLog(G2U(result.jsonMsg.c_str()));
	}
}

void LoginScene::gameSettingCallback(const CCPomeloRequestResult& result)
{
	CCLOG("LoginScene-gameSettingCallback = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		float commissionCharge = d["setting"]["commissionCharge"].GetFloat();
		float commissionLimit = d["setting"]["commissionLimit"].GetFloat();
		int betCloseTime = d["setting"]["betCloseTime"].GetInt();

		std::vector<int> commissionFreeDay;// 佣金提现免费日 

		if (d["setting"]["commissionFreeDay"].IsArray())
		{
			for (int i = 0; i < d["setting"]["commissionFreeDay"].Size(); i++)
			{
				commissionFreeDay.push_back(d["setting"]["commissionFreeDay"][i].GetInt());
			}
		}

		GameManager::getInstance()->_gameSetStruct = GameSetStruct(commissionCharge, commissionLimit, commissionFreeDay, betCloseTime);
		
		rapidjson::Document document4;
		document4.SetObject();
		//rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
		rapidjson::StringBuffer buffer4;
		rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
		document4.Accept(bb4);

		PomeloInstance->request("game.gameHandler.getTodayBetInfo", buffer4.GetString(), CC_CALLBACK_1(LoginScene::getMyBetInfo, this));
	}
	else
	{
		_loadingLayer->removeLoading();
		DiaLogLayer::create()->setLog(G2U(result.jsonMsg.c_str()));
	}
}

void LoginScene::getAllBetInfo(const CCPomeloRequestResult& result)
{
	CCLOG("LoginScene-getAllBetInfo = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		rapidjson::Document document4;
		document4.SetObject();
		//rapidjson::Document::AllocatorType& allocator4 = document4.GetAllocator();
		rapidjson::StringBuffer buffer4;
		rapidjson::Writer<rapidjson::StringBuffer> bb4(buffer4);
		document4.Accept(bb4);

		PomeloInstance->request("game.gameHandler.getMyBetInfo", buffer4.GetString(), CC_CALLBACK_1(LoginScene::getMyBetInfo, this));
	}
}

void LoginScene::getMyBetInfo(const CCPomeloRequestResult& result)
{
	CCLOG("LoginScene-getMyBetInfo = %s", result.jsonMsg.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.jsonMsg.c_str());

	if (d.HasParseError())
	{
		//CCLOG("queryEntryCallback = %d\n", d.GetParseError());
	}
	else if (d["code"].GetInt() == 200)
	{
		if (d["betInfo"].IsArray())
		{
			GameInstance->_historyList.clear();

			for (int i = 0; i < d["betInfo"].Size(); i++)
			{
				std::string opencode = d["betInfo"][i]["openCode"].GetString();

				if (opencode == "0")
				{
					continue;
				}

				std::vector<std::string> openCodeList;

				std::stringstream ss;
				for (int i = 0; i < 5; i++)
				{
					ss.str("");
					char ffas = opencode.at(i * 2);
					ss << ffas;
					std::string asfd = ss.str();
					openCodeList.push_back(asfd);
				}

				int opentype = 0;
				int list0 = Value(openCodeList[0]).asInt();
				int list4 = Value(openCodeList[4]).asInt();

				if (list0 > list4)
				{
					opentype = 0;
				}
				else if (list0 == list4)
				{
					opentype = 2;
				}
				else if (list0 < list4)
				{
					opentype = 1;
				}

				HistoryStruct historyStruct = HistoryStruct(d["betInfo"][i]["expect"].GetString(), 
					__String::createWithFormat("%.2f", d["betInfo"][i]["betDragon"].GetDouble())->getCString(),
					__String::createWithFormat("%.2f", d["betInfo"][i]["betEqual"].GetDouble())->getCString(),
					__String::createWithFormat("%.2f", d["betInfo"][i]["betTiger"].GetDouble())->getCString(),
					__String::createWithFormat("%.2f", d["betInfo"][i]["odds"]["dragon"].GetDouble())->getCString(),
					__String::createWithFormat("%.2f", d["betInfo"][i]["odds"]["equal"].GetDouble())->getCString(),
					__String::createWithFormat("%.2f", d["betInfo"][i]["odds"]["tiger"].GetDouble())->getCString(),
					__String::createWithFormat("%.2f", d["betInfo"][i]["winLose"].GetDouble())->getCString(),
					Value(opentype).asString());

				GameInstance->_historyList.push_back(historyStruct);
			}
		}
		Director::getInstance()->replaceScene(MainScene::createScene());
	}
	else
	{
		_loadingLayer->removeLoading();
		DiaLogLayer::create()->setLog(G2U(result.jsonMsg.c_str()));
	}
}
