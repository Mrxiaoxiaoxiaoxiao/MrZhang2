#pragma once
#include "FactoryCodec.h"


//工厂类  只负责创建请求报文编解码对象
class RequestFactory : public FactoryCodec
{
public:
	RequestFactory();
	~RequestFactory();

	virtual Codec *createCodec(void *arg = NULL);
};

