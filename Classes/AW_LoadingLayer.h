#pragma once

#include "AW_MessageHead.h"

class LoadingLayer : public Layer
{
public:

	virtual bool init();
	void removeLoading();
	void addIndex();

	CREATE_FUNC(LoadingLayer);

private:

	int _index;
};