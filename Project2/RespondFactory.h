#pragma once
#include "FactoryCodec.h"

//工厂模式  工厂类  主要负责创建响应报文编解码对象
class RespondFactory :public FactoryCodec
{
public:
	RespondFactory();
	~RespondFactory();
	//工厂类中创建对象方法  编解码对象
	virtual Codec *createCodec(void *arg = NULL);
};

