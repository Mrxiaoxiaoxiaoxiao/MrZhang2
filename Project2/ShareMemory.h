#pragma once
#include <iostream>
#include <sys/shm.h>
#include <sys/ipc.h>
using namespace std;

const char RandX = 'x';
//共享内存基类
class ShareMemory
{
public:
    //打开共享内存
	ShareMemory(int key);
    //打开共享内存
	ShareMemory(const char* name);
    //创建共享内存
	ShareMemory(int key, int size);
    //创建共享内存
	ShareMemory(const char* name, int size);
    //析构函数
	virtual ~ShareMemory();

    //关联共享内存
	void* mapShm();
    //解除关联共享内存
	int unmapShm();
    //删除共享内存
	int delShm();


private:
    //获取共享内存ID
	int getShmID(key_t key, int shmSize, int flag);

private:
	int m_shmID;
	void* m_shmAddr = NULL;

};

