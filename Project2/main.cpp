#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "ClientOperation.h"
#include "RequestCodec.h"

using namespace std;

//显示一个菜单
int showMenu(void)
{
    int choice;

    cout << "========================================" << endl;
    cout << "========================================" << endl;
    cout << "==========1. 密钥协商          =========" << endl;
    cout << "==========2. 密钥校验          =========" << endl;
    cout << "==========3. 密钥注销          =========" << endl;
    cout << "==========4. 密钥查看          =========" << endl;
    cout << "==========0. 退出              =========" << endl;
    cout << "========================================" << endl;
    cout << "========================================" << endl;
    cout << "请选择: ";
    
    cin >> choice;

    //清空输入缓冲区
    while('\n' != getchar())
        /*do nothing*/;

    return choice;
}

int main(void)
{
    int ret = -1;

    //客户端基本信息
    ClientInfo info;
    strcpy(info.clientID, "1111"); 
    strcpy(info.serverID, "0001"); 
    strcpy(info.authCode, "1111"); 
    strcpy(info.serverIP, "192.168.11.54"); 
    info.serverPort = 10086;
    info.maxNode = 1;   //客户端只有一个网点 默认为1
    info.shmKey = 0x55;

    //构造一个客户端对象
    ClientOperation client(&info);

#if 0
    char buf[128];
    client.getRandString(128, buf);
    cout << "buf:" << buf << endl;
#endif

    while(1)
    {
        //清屏
        system("clear");

        //显示菜单
        ret = showMenu(); 
        if (0 == ret) 
        {
            cout << "很遗憾的离开了..." << endl;
            break;
        }
    
        switch(ret)
        {
            //密钥协商
				case RequestCodec::NewOrUpdate:
                cout << "密钥协商" << endl;
				ret = client.secKeyAgree();
                break;

            //密钥校验
            case RequestCodec::Check:
                cout << "密钥校验" << endl;
				ret = client.secKeyCheck();
                break;

            //密钥注销
            case RequestCodec::Revoke:
                cout << "密钥注销" << endl;
				ret = client.secKeyRevoke();
                break;
            
            //密钥查看
            case RequestCodec::View:
                cout << "密钥查看" << endl;
				ret = client.secKeyView();
                break;
        
            default:
                cout << "无效的选择" << endl; 
        }

        cout << "按下任意键继续..." << endl;
        getchar();
    }


    return 0;
}
