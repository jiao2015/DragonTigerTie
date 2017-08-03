#pragma once

#include "AW_MessageHead.h"

class CCPomeloRequestResult;
class RegisterLayer :public Layer
{
public:

	virtual bool init();
	RegisterLayer();
	bool isIdTrue(const std::string& userid, REGEX_TYPE type);

	CREATE_FUNC(RegisterLayer);

private:

	TextField *_tfName;
	TextField *_tfNickname;
	TextField *_tfPassword;
	TextField *_tfRePassword;

	Button *_btnRegister;
};