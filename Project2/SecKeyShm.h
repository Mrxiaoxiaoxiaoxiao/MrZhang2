#pragma once
#include "ShareMemory.h"

//网点密钥信息类
class NodeSHMInfo
{
public:
	int status;     //密钥状态
	int seckeyID;   //密钥编号
	char clientID[12];  //服务端ID
	char serverID[12];  //客户端ID
	char secKey[128];   //密钥信息
};

//共享内存业务流类
class SecKeyShm : public ShareMemory
{
public:
	SecKeyShm(int key);
	SecKeyShm(int key, int maxNode);
	SecKeyShm(const char* pathName);
	SecKeyShm(const char* pathName, int maxNode);
	~SecKeyShm();

	int shmWrite(NodeSHMInfo* pNodeInfo);
	int shmRead(const char* clientID, const char* serverID, NodeSHMInfo* pNodeInfo);

private:
	int m_maxNode;      //最大网点数
};

