#include "RespondFactory.h"
#include "RespondCodec.h"


RespondFactory::RespondFactory(): FactoryCodec()
{
}


RespondFactory::~RespondFactory()
{
}

//创建对象的方法
Codec * RespondFactory::createCodec(void * arg)
{
	Codec *codec = NULL;

	if (NULL == arg)
	{
		codec = new RespondCodec();
	}
	else
	{
		codec = new RespondCodec((RespondMsg*) arg);
	}

	return codec;
}
