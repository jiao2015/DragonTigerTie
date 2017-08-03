#pragma once

#include "cocos2d.h"
#include "AW_MessageHead.h"
#include "PomeloWrapper.h"

class LoadingLayer;
class LoginScene : public cocos2d::Layer
{
public:

	static Scene *createScene();

	virtual bool init();

	void btnClickCallback(Ref * pSender);

	void asynConnectCallback(int result);
	void registerConnectCallback(int result);
	void queryEntryCallback(const CCPomeloRequestResult& result);
	void connectorCallback(const CCPomeloRequestResult& result);
	void betCallback(const CCPomeloRequestResult& result);
	void getBankCardCallback(const CCPomeloRequestResult& result);
	void gameSettingCallback(const CCPomeloRequestResult& result);
	void getAllBetInfo(const CCPomeloRequestResult& result);
	void getMyBetInfo(const CCPomeloRequestResult& result);

	CREATE_FUNC(LoginScene);
	LoginScene();
	~LoginScene();
private:

	Button *_btnLogin;
	Button *_btnRegister;
	Button *_btnEnglish;
	Button *_btnWX;
	Button *_btnForget;

	LoadingLayer *_loadingLayer;

	TextField *_tfName;
	TextField *_tfPassWord;

	RegisterInfo _registerInfo;
};