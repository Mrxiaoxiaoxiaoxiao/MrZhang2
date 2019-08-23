#include "SecKeyShm.h"
#include <string.h>
#include <stdlib.h>

SecKeyShm::SecKeyShm(int key):ShareMemory(key)
{

}

SecKeyShm::SecKeyShm(int key, int maxNode):ShareMemory(key, maxNode * sizeof(NodeSHMInfo)), m_maxNode(maxNode)
{

}

SecKeyShm::SecKeyShm(const char* pathName):ShareMemory(pathName)
{

}

SecKeyShm::SecKeyShm(const char* pathName, int maxNode):ShareMemory(pathName, maxNode * sizeof(NodeSHMInfo)), m_maxNode(maxNode)
{

}

SecKeyShm::~SecKeyShm()
{

}


//将网点密钥信息写入共享内存
int SecKeyShm::shmWrite(NodeSHMInfo* pNodeInfo)
{
    int i = 0;
    void *addr = NULL;
    NodeSHMInfo *tmp = NULL;
    NodeSHMInfo tmpNode;

    //1. 关联共享内存
    addr = mapShm();        
    if (NULL == addr)
    {
        cout << "mapShm failed.." << endl; 
        return -1;
    }

    //2. 写网点密钥信息到共享内存
    //1) 第一种情形 共享内存中存在网点对应的密钥信息
    tmp = static_cast<NodeSHMInfo*>(addr);
    for (i = 0; i < m_maxNode; i++) 
    {
        //根据serverID  ClientId来判断对应的解释是否存在 
        if ((strcmp(tmp->serverID, pNodeInfo->serverID) == 0) && (strcmp(tmp->clientID, pNodeInfo->clientID) == 0))
        {
            cout << "找到对应网点密钥信息, 现在覆盖原来网点密钥信息" << endl; 
            memcpy(tmp, pNodeInfo, sizeof(NodeSHMInfo));
            break;
        }
        tmp = tmp + 1;
    }

    //2) 第二种情形 共享内存中不存在网点对应的密钥信息
    if (i == m_maxNode)
    {
        //memset((void*)&tmpNode, 0, sizeof(tmpNode));
        memset(static_cast<void*>(&tmpNode), 0, sizeof(tmpNode));
        tmp = static_cast<NodeSHMInfo*>(addr); 

        //遍历共享内存 寻找一个空白区域
        for (i = 0; i < m_maxNode; i++)
        {
            //cout << "i = " << i << endl;
            if (memcmp(&tmpNode, tmp, sizeof(tmpNode)) == 0)
            {
                cout << "找到空白区域, 将网点密钥信息写入共享内存" << endl; 
                memcpy(tmp, pNodeInfo, sizeof(NodeSHMInfo));
                break;
            }
            tmp = tmp + 1;
        }

        if (i == m_maxNode)
        {
            cout << "共享内存已经写满了, 请联系管理员" << endl; 
        }
    }
    //3. 取消关联共享内存
    unmapShm();

    return 0;
}

//从共享内存总读取网点密钥信息
int SecKeyShm::shmRead(const char* clientID, const char* serverID, NodeSHMInfo* pNodeInfo)
{
    int i = 0;

    NodeSHMInfo *addr = NULL;
    NodeSHMInfo *tmpNode = NULL;
    //1. 关联共享内存
    addr = static_cast<NodeSHMInfo*>(mapShm()); 

    //2. 从共享内存中查找对应的节点
    tmpNode = addr;
    for (i = 0; i < m_maxNode; i++) 
    {
        if (strcmp(clientID, tmpNode->clientID) == 0 && strcmp(serverID, tmpNode->serverID) == 0)
        {
            cout << "找到对应网点的密钥信息" << endl; 
            memcpy(pNodeInfo, tmpNode, sizeof(NodeSHMInfo));
            break;
        }
    }

    if (i == m_maxNode)
    {
        cout << "共享内存中不存在对应的网点" << endl; 
    }


    //3. 取消关联共享内存
    unmapShm();
}

