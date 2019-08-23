#include "RequestFactory.h"
#include "RequestCodec.h"

RequestFactory::RequestFactory():FactoryCodec()
{
}


RequestFactory::~RequestFactory()
{
}

//创建请求报文编解码对象
Codec * RequestFactory::createCodec(void * arg)
{
	Codec *codec = NULL;

	if (NULL == arg)
	{
		codec = new RequestCodec();
	}
	else
	{
		codec = new RequestCodec((RequestMsg*)arg);
	}

	return codec;
}
