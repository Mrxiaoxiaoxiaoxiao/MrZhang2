#define _CRT_SECURE_NO_WARNINGS
#include "RespondCodec.h"

#include <iostream>

using namespace std;


//构造函数
RespondCodec::RespondCodec():Codec()
{
	//分配内存
	m_msg = new  RespondMsg;
	if (NULL == m_msg)
	{
		cout << "new RespondMsg failed.." << endl;

		return;
	}
}

RespondCodec::RespondCodec(RespondMsg * msg):Codec()
{
	//分配内存
	m_msg = new RespondMsg;
	if (NULL == m_msg)
	{
		cout << "new RespondMsg failed.." << endl;

		return;
	}

	//赋值
	m_msg->rv = msg->rv;
	strcpy(m_msg->clientId, msg->clientId);
	strcpy(m_msg->serverId, msg->serverId);
	strcpy((char *)m_msg->r2, (char *)msg->r2);

	m_msg->seckeyid = msg->seckeyid;
}


//析构函数
RespondCodec::~RespondCodec()
{
	delete m_msg;
}


int RespondCodec::msgEncode(char ** outData, int & len)
{
	int ret = 0;

	//1. 编码rv
	ret = writeHeadNode(m_msg->rv);
	if (0 != ret)
	{
		cout << "writeHeadNode rv failed..." << endl;
		return ret;
	}

	//2. 编码clientId
	ret = writeNextNode(m_msg->clientId, strlen(m_msg->clientId));
	if (0 != ret)
	{
		cout << "writeHeadNode clientId failed..." << endl;
		return ret;
	}

	//3. 编码serverId
	ret = writeNextNode(m_msg->serverId, strlen(m_msg->serverId));
	if (0 != ret)
	{
		cout << "writeHeadNode serverId failed..." << endl;
		return ret;
	}

	//4. 编码r2
	ret = writeNextNode((char*)m_msg->r2, strlen((char *)m_msg->r2));
	if (0 != ret)
	{
		cout << "writeHeadNode r2 failed..." << endl;
		return ret;
	}

	//5. 编码seckeyid
	ret = writeNextNode(m_msg->seckeyid);
	if (0 != ret)
	{
		cout << "writeHeadNode seckeyId failed..." << endl;
		return ret;
	}

	//6. 编码大结构体
	ret = packSequence(outData, len);
	if (0 != ret)
	{
		cout << "packSequence failed..." << endl;
		return ret;
	}

	//7. 传出 

	//8. 释放链表空间
	freeSequence();

	return 0;
}

//响应报文解码函数
void * RespondCodec::msgDecode(char * inData, int inLen)
{
	int ret = 0;

	//清零
	memset(m_msg, 0, sizeof(RespondMsg));

	//1. 解码大结构体
	ret = unpackSequence(inData, inLen);
	if (0 != ret)
	{
		cout << "packSequence failed..." << endl;
		return NULL;
	}

	//2. 解码rv
	ret = readHeadNode(m_msg->rv);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//3. 解码clientId
	ret =  readNextNode(m_msg->clientId);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//4. 解码serverId
	ret = readNextNode(m_msg->serverId);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//5. 解码r2
	ret = readNextNode((char *)m_msg->r2);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//6. 解码seckeyid
	ret = readNextNode(m_msg->seckeyid);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//7. 释放链表空间
	freeSequence();

	return m_msg;
}

int RespondCodec::msgMemFree(void ** point)
{
	return 0;
}
