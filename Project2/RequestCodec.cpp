#define _CRT_SECURE_NO_WARNINGS
#include "RequestCodec.h"

//无参构造函数
RequestCodec::RequestCodec()
{
	m_msg = new RequestMsg;
}

//带参数的构造函数
RequestCodec::RequestCodec(RequestMsg * msg)
{
	//分配空间
	m_msg = new RequestMsg;

	//赋值
	//报文类型
	m_msg->cmdType = msg->cmdType;
	//客户端编号
	strcpy(m_msg->clientId, msg->clientId);
	//服务端编号
	strcpy(m_msg->serverId, msg->serverId);
	//认证码
	strcpy(m_msg->authCode, msg->authCode);
	//随机数
	memcpy(m_msg->r1, msg->r1, sizeof(msg->r1));
}

//析构函数
RequestCodec::~RequestCodec()
{
	delete m_msg;
}

int RequestCodec::msgEncode(char ** outData, int & len)
{
	//1. 编码cmdType
	writeHeadNode(m_msg->cmdType);

	//2. 编码clientId
	writeNextNode(m_msg->clientId, strlen(m_msg->clientId));

	//3. 编码authCode
	writeNextNode(m_msg->authCode, strlen(m_msg->authCode));

	//4. 编码serverId
	writeNextNode(m_msg->serverId, strlen(m_msg->serverId));

	//5. 编码r1
	writeNextNode(m_msg->r1, sizeof(m_msg->r1));

	//6. 编码大结构体
	packSequence(outData, len);	//1. free

	//7. 释放链表
	freeSequence();

	return 0;
}


void * RequestCodec::msgDecode(char * inData, int inLen)
{
	//清零
	memset(m_msg, 0, sizeof(struct RequestMsg));

	//1. 解码大结构体  得到一个链表
	unpackSequence(inData, inLen);

	//2. 解码cmdType
	readHeadNode(m_msg->cmdType);

	//3. 解码clientId
	readNextNode(m_msg->clientId);

	//4. 解码authCode
	readNextNode(m_msg->authCode);

	//5. 解码serverId
	readNextNode(m_msg->serverId);

	//6. 解码r1
	readNextNode(m_msg->r1);

	//7. 释放链表
	freeSequence();

	return m_msg;
}

int RequestCodec::msgMemFree(void ** point)
{

	return 0;
}
