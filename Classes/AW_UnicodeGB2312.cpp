#include "AW_UnicodeGB2312.h"
#include "cocos2d.h"

std::string UnicodeGB2312::U2G(const char* utf8)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	std::string strRet(str);
	//cocos2d::log(strRet.c_str());
	return strRet;
#else
	return utf8;
#endif
}

std::string UnicodeGB2312::G2U(const char* gb2312)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	std::string strRet(str);
	//cocos2d::log(strRet.c_str());
	return strRet;
#else
	return gb2312;
#endif
}




