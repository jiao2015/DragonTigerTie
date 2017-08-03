#pragma once

#include "AW_MessageHead.h"

class BoardCast : public Layer
{
public:

	BoardCast();
	CREATE_FUNC(BoardCast);
	virtual bool init();

	virtual void update(float delta);

	void stop();
	void start();
	void showBoardCast(std::string msg);

private:

	LabelTTF* _text;
	bool _isStop;
};