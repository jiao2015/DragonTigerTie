//
//  CCPomeloWrapper.h
//
//  Created by laoyur@126.com on 13-11-22.
//
//  A simple libpomelo wrapper for cocos2d-x
//  
//  Version history:
//  0.9.3   2014-03-04  �޸�0.9.2��cocos2dx-2.x��stop()���ܵ��±�����bug
//  0.9.2   2014-03-03  �޸�0.9�汾��һ������������bug
//  0.9.1   2014-02-28  ��ʱ�ƹ�pc_client_destroy()��ĳЩ����¹�ס���̵߳�bug��
//                      ע�⣺Ŀǰֻ��һ������������Ծɿ��ܴ����ڴ�й¶��δ֪bug��
//                      ��Ҫ�ٷ���һ������libpomelo�Ľ�׳�ԡ�
//  0.9     2014-01-26  1.cocos2dx_3.0 && c++11 supported
//                      2.�������ע�ͣ���������û�����ʹ��
//  < 0.9   2013    first released version

#ifndef __CCPomeloWrapper__
#define __CCPomeloWrapper__

#include "cocos2d.h"

#if COCOS2D_VERSION >= 0x00030000
#define CCX3 1  //cocos2dx 3.0+
#include <functional>
#else
#define CCX3 0  //cocos2dx 2.x
#endif


enum CCPomeloStatus
{
	EPomeloStopped = 0,     //�����Ѿ��Ͽ�
	EPomeloConnecting = 1,  //����������
	EPomeloConnected = 2,   //�����Ѿ�ȷ��

	/*���ڶϿ����ӡ��ڴ�״̬�£�CCPomeloWrapper������client api���ᱻ���ԡ�
	���ִ�״̬��һ�����ͳ����ǣ������򱻶���stop()���ã�ʹ������δ��ɵ�request/notify
	�Ļص����Դ���������Щ�ص��з�������е�client api�ĵ��ô��߼��϶�����Ч�ġ�
	This status means CCPomeloWrapper is shutting down.
	Pending request/notify callbacks will be fired if stop() called by
	user or CCPomeloWrapper itself, and then the status will change to
	EPomeloStopped.
	You must NOT call any client api until the status becomes EPomeloStopped.
	*/
	EPomeloStopping = 3
};


class CCPomeloImpl;

class CCPomeloRequestResult
{
public:
	int status;         //status code
	std::string requestRoute;
	std::string jsonMsg;

private:
	CCPomeloRequestResult();
	friend class CCPomeloImpl;
};

class CCPomeloNotifyResult
{
public:
	int status;
	std::string notifyRoute;

private:
	CCPomeloNotifyResult();
	friend class CCPomeloImpl;
};

class CCPomeloEvent
{
public:
	std::string event;
	std::string jsonMsg;
private:
	CCPomeloEvent();
	friend class CCPomeloImpl;
};

#if CCX3
typedef std::function<void (int)> PomeloAsyncConnCallback;
typedef std::function<void(const CCPomeloRequestResult&)> PomeloReqResultCallback;
typedef std::function<void(const CCPomeloNotifyResult&)> PomeloNtfResultCallback;
typedef std::function<void(const CCPomeloEvent&)> PomeloEventCallback;
typedef std::function<void(void)> PomeloDisconnectedCallback;
#else
typedef void (cocos2d::CCObject::*PomeloAsyncConnHandler)(int);
typedef void (cocos2d::CCObject::*PomeloReqResultHandler)(const CCPomeloRequestResult&);
typedef void (cocos2d::CCObject::*PomeloNtfResultHandler)(const CCPomeloNotifyResult&);
typedef void (cocos2d::CCObject::*PomeloEventHandler)(const CCPomeloEvent&);

#define pomelo_async_conn_cb_selector(_SEL) (PomeloAsyncConnHandler)(&_SEL)
#define pomelo_req_result_cb_selector(_SEL) (PomeloReqResultHandler)(&_SEL)
#define pomelo_ntf_result_cb_selector(_SEL) (PomeloNtfResultHandler)(&_SEL)
#define pomelo_listener_cb_selector(_SEL) (PomeloEventHandler)(&_SEL)
#endif

class CCPomeloWrapper :
#if CCX3
	public cocos2d::Object
#else
	public cocos2d::CCObject
#endif
{
public:
	static CCPomeloWrapper* getInstance();
	~CCPomeloWrapper();

public: //client APIs
	//get the current connection status
	//��ȡ��ǰ����״̬
	CCPomeloStatus status() const;

	//@return: 0--connect succeeded; others--connect failed
	//!!!use the async version in production!!!
	//ͬ�����ӷ�����
	int connect(const char* host, int port);

#if CCX3
	int connectAsnyc(const char* host, int port, const PomeloAsyncConnCallback& callback);
#else
	//@return: 0--connect request succeeded; others--connect request failed
	//�첽���ӷ�����
	int connectAsnyc(const char* host, int port, cocos2d::CCObject* pCallbackTarget, PomeloAsyncConnHandler pCallbackSelector);
#endif

	//stop the current connection
	//�Ͽ���ǰ����
	void stop();

#if CCX3
	int setDisconnectedCallback(const PomeloDisconnectedCallback& callback);
#else
	//callback when connection lost
	//������Ӷ�ʧ�¼�
	int setDisconnectedCallback(cocos2d::CCObject* pTarget, cocos2d::SEL_CallFunc pSelector);
#endif

#if CCX3
	int request(const char* route, const std::string& msg, const PomeloReqResultCallback& callback);
#else
	//send request to server
	//@return: 0--request sent succeeded; others--request sent failed
	//����request
	int request(const char* route, const std::string& msg, cocos2d::CCObject* pCallbackTarget, PomeloReqResultHandler pCallbackSelector);
#endif

#if CCX3
	int notify(const char* route, const std::string& msg, const PomeloNtfResultCallback& callback);
#else
	//send notify to server
	//@return: 0--notify sent succeeded; others--notify sent failed
	//����notify
	//�������˻������ʣ�notify����û��reply���Ϊë���ﻹҪ��callback
	//�����callbackֻ�ǡ�notify���ͳɹ�����callback
	int notify(const char* route, const std::string& msg, cocos2d::CCObject* pCallbackTarget, PomeloNtfResultHandler pCallbackSelector);
#endif

#if CCX3
	int addListener(const char* event, const PomeloEventCallback& callback);
#else
	//listen to event
	//only one listener allowed for one event currently
	//@return: 0--add listener succeeded; others--add listener failed
	//�����¼�������ͬһ���¼���Ŀǰֻ֧��һ���۲��ߡ�Ҳ����˵��ͬһ�¼���ζ��ģ�ǰһ���������Զ����Ƴ���
	int addListener(const char* event, cocos2d::CCObject* pCallbackTarget, PomeloEventHandler pCallbackSelector);
#endif

	//remove listener for specific event
	//�Ƴ��¼�����
	void removeListener(const char* event);

	//remove all listeners for all events
	//�Ƴ������¼�����
	void removeAllListeners();


private:
	CCPomeloWrapper();

private:
	CCPomeloImpl*   _theMagic;
	friend class CCPomeloImpl;
};

#endif /* defined(__CCPomeloWrapper__) */
