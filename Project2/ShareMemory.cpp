#include "ShareMemory.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//打开共享内存
ShareMemory::ShareMemory(int key)
{
    //打开共享内存
    m_shmID = shmget(key, 0, 0);
    if (-1 == m_shmID)
    {
        perror("shmget"); 
        //写日志
        return;
    }
}

//打开共享内存
ShareMemory::ShareMemory(const char* name)
{
    //获取对应的key
    key_t key = ftok(name, RandX);
    if (-1 == key)
    {
        perror("ftok"); 
        //写日志
        return;
    }

    //打开共享内存
    m_shmID = shmget(key, 0, 0);
    if (-1 == m_shmID)
    {
        perror("shmget"); 
        return;
    }
    
}

//创建共享内存
ShareMemory::ShareMemory(int key, int size)
{
    getShmID(key, size, IPC_CREAT | 0644);
}

//创建共享内存
ShareMemory::ShareMemory(const char* name, int size)
{
    key_t key = -1;
    //获取对应的key 
    key = ftok(name, RandX);
    if (-1 == key)
    {
        perror("ftok"); 
        return;
    }

    getShmID(key, size, IPC_CREAT | 0644);

}

//析构函数
ShareMemory::~ShareMemory()
{

}


//关联共享内存
void* ShareMemory::mapShm()
{
    //关联共享内存
    m_shmAddr = shmat(m_shmID, NULL, 0);
    if ((void *)-1 == m_shmAddr)
    {
        perror("shmat"); 
        return NULL;
    }

    return m_shmAddr;
}

//解除关联共享内存
int ShareMemory::unmapShm()
{
    shmdt(m_shmAddr);
}

//删除共享内存
int ShareMemory::delShm()
{
    int ret = -1;
    ret = shmctl(m_shmID, IPC_RMID, NULL);
    if (-1 == ret)
    {
        perror("shmclt"); 
        return -1;
    }

    return ret;
}

//获取共享内存ID
int ShareMemory::getShmID(key_t key, int shmSize, int flag)
{
    if (key <= 0 || shmSize <= 0)
    {
        //写日志 
    }

    //如果共享内存存在就直接打开
    if (-1 == (m_shmID = shmget(key, 0, 0)))
    {

        //通过key获取ID
        m_shmID = shmget(key, shmSize, flag);
        if (-1 == m_shmID)
        {
            perror("shmget"); 
            return -1;
        }

        //清零
        //关联共享内存 默认读写
        m_shmAddr = shmat(m_shmID, NULL, 0);
        if ((void *)-1 == m_shmAddr)
        {
            perror("shmat"); 
            //写日志
        }

        //清零
        memset(m_shmAddr, 0, shmSize);

        //解除关联
        shmdt(m_shmAddr);
    }

	return 0;
}


