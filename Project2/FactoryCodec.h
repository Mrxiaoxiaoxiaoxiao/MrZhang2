#pragma once

#include "RequestCodec.h"
#include "RespondCodec.h"

//工厂模式基类  工厂类
class FactoryCodec
{
public:
	FactoryCodec();
	~FactoryCodec();

	//工厂类中创建对象方法  编解码对象
	virtual Codec *createCodec(void *arg = NULL);
};

