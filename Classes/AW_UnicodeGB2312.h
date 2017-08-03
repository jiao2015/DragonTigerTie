#pragma once

#include <string>

class UnicodeGB2312
{
public:

	static std::string U2G(const char* utf8);
	static std::string G2U(const char* gb2312);

private:

};
