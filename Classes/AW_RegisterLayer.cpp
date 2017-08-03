#include "AW_RegisterLayer.h"
#include "PomeloWrapper.h"
#include "AW_DiaLogLayer.h"
#include "AW_UnicodeGB2312.h"
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include <regex.h>
#endif
#include <regex>

RegisterLayer::RegisterLayer()
	:
	_tfName(nullptr)
	,_tfNickname(nullptr)
	,_tfPassword(nullptr)
	,_tfRePassword(nullptr)
	, _btnRegister(nullptr)
{

}

bool RegisterLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto registerNode = CSLoader::createNode("RegisterLayer.csb");
	addChild(registerNode);

	_tfName = registerNode->getChildByName<TextField *>("TextField_name");
	_tfNickname = registerNode->getChildByName<TextField *>("TextField_nickname");
	_tfPassword = registerNode->getChildByName<TextField *>("TextField_passWord");
	_tfRePassword = registerNode->getChildByName<TextField *>("TextField_repassWord");

	_btnRegister = registerNode->getChildByName<Button *>("Button_register");
	_btnRegister->addClickEventListener([this](Ref *pSender){
		SoundManger::getInstance()->playSound("Sound/click.wav");
		log("isIdTrue(_tfName) = %d, isIdTrue(_tfNickname) = %d , isIdTrue(_tfPassword) = %d, isIdTrue(_tfRePassword) = %d", isIdTrue(_tfName->getString(), REGEX_CODE), isIdTrue(_tfNickname->getString(), REGEX_CODE), isIdTrue(_tfPassword->getString(), REGEX_CODE), isIdTrue(_tfRePassword->getString(), REGEX_CODE));
		if (isIdTrue(_tfName->getString(), REGEX_CODE) && isIdTrue(_tfPassword->getString(), REGEX_CODE) && isIdTrue(_tfNickname->getString(), REGEX_CODE) && isIdTrue(_tfRePassword->getString(), REGEX_CODE) && (_tfRePassword->getString() == _tfPassword->getString()))
		{
			RegisterInfo registerInfo = RegisterInfo(_tfName->getString(), _tfPassword->getString(), _tfNickname->getString());
			DisPatcher->dispatchCustomEvent("RegisterCall", &registerInfo);
		}
		else if (_tfPassword->getString() != _tfRePassword->getString())
		{
			DiaLogLayer::create()->setLog(G2U("两次输入密码不同"));
		}
		else
		{
			DiaLogLayer::create()->setLog(G2U("信息格式不对"));
		}
		
	});

	auto btnClose = registerNode->getChildByName<Button *>("Button_close");
	btnClose->addClickEventListener([this](Ref *pSender){
		SoundManger::getInstance()->playSound("Sound/click.wav");
		this->removeFromParent();
	});

	return true;
}

bool RegisterLayer::isIdTrue(const std::string& userid, REGEX_TYPE type)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)

	char ss[100] = {};
	sprintf(ss, "%s", userid.c_str());
	regmatch_t pmatch[16];
	regex_t match_regex;

	if (type == REGEX_PHONE)
	{
		regcomp(&match_regex,
			"1[]3|5|7|8|[-][]0-9[-]{9}",
			REG_EXTENDED);
		if (regexec(&match_regex, ss, 16, pmatch, 0) != 0)
		{
			return false;
		}
		regfree(&match_regex);

	}
	else if (type == REGEX_CODE)
	{
		//		const std::regex pattern("^[a-zA-Z0-9]\\w{4,19}$");       //
		//
		//		return std::regex_match(userid, pattern);
		//        regcomp( &match_regex,
		//                "^[]a-zA-Z0-9[-]\\w{4,19}$",
		//                REG_EXTENDED );

		regcomp(&match_regex,
			"^[]a-zA-Z0-9~!@#$%^&*()_+`={}:;'<>?,./\\[-]{6,20}$",
			REG_EXTENDED);

		if (regexec(&match_regex, ss, 16, pmatch, 0) != 0)
		{
			return false;
		}
		regfree(&match_regex);
	}
	else if (type == REGEX_MSG)
	{
		//		const std::regex pattern("^[0-9]{4}$");       //
		//
		//		return std::regex_match(userid, pattern);
		regcomp(&match_regex,
			"^[]0-9[-]{4}$",
			REG_EXTENDED);
		if (regexec(&match_regex, ss, 4, pmatch, 0) != 0)
		{
			return false;
		}
		regfree(&match_regex);
	}
	else
	{
		log("¥ÌŒÛ");
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		if (type == REGEX_PHONE)
		{
			const std::regex pattern("1[3|5|7|8|][0-9]{9}");       //

			return std::regex_match(userid, pattern);
		}
		else if (type == REGEX_CODE)
		{
			const std::regex pattern("^[a-zA-Z0-9]\\w{5,19}$");       //

			return std::regex_match(userid, pattern);
		}
		else if (type == REGEX_MSG)
		{
			const std::regex pattern("^[0-9]{4}$");       //

			return std::regex_match(userid, pattern);
		}
		else
		{
			log("¥ÌŒÛ");
		}
	}
#endif

	return true;
}