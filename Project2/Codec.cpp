#include "Codec.h"

Codec::Codec():SequenceASN1()
{

}

Codec::~Codec()
{

}

int Codec::msgEncode(char ** outData, int & len)
{
	return 0;
}

void * Codec::msgDecode(char * inData, int inLen)
{
	//return nullptr;
    return NULL;
}

int Codec::msgMemFree(void ** point)
{
	return 0;
}
