#include "AW_LoadingLayer.h"

bool LoadingLayer::init()
{
	AW_INIT_IF(!Layer::init());

	auto loadNode = CSLoader::createNode("LoadingLayer.csb");
	addChild(loadNode);

	_index = 0;

	return true;
}

void LoadingLayer::removeLoading()
{
	_index--;
	if (_index == 0)
	{
		this->setVisible(false);
	}
}

void LoadingLayer::addIndex()
{
	this->setVisible(true);
	_index++;
}