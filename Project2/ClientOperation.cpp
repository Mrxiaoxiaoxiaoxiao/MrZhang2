#include "ClientOperation.h"
#include <cstring>
#include "RequestCodec.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include "RequestFactory.h"
#include "RespondFactory.h"
#include "SecKeyShm.h"

//构造函数
ClientOperation::ClientOperation(ClientInfo *info)
{
    //给数据成员赋值
    m_info = new ClientInfo;
    if (NULL == m_info)
    {
        cout << "new ClientInfo failed.." << endl;
        return;
    }

    //服务端配置信息赋值
    memcpy(m_info, info, sizeof(ClientInfo));

    //new共享内存对象
    m_shm = new SecKeyShm(info->shmKey, info->maxNode * sizeof(NodeSHMInfo));
    if (NULL == m_shm)
    {
        cout << "new SecKeyShm faild..." << endl; 
        return;
    }

    cout << "密钥协商客户端初始化ok..." << endl;
}

//析构函数
ClientOperation::~ClientOperation()
{
    delete m_info;
    delete m_shm;
}


// 秘钥协商
int ClientOperation::secKeyAgree()
{
	int ret = -1;
	char buf[32];

	int bufLen = -1;

	RequestMsg requestMsg;

	//1. 组织密钥协商请求报文
	memset(&requestMsg, 0, sizeof(RequestMsg));
	requestMsg.cmdType = RequestCodec::NewOrUpdate; //密钥协商
	strcpy(requestMsg.clientId, m_info->clientID);	//客户端编号
	strcpy(requestMsg.serverId, m_info->serverID);	//服务端编号

	//生成一个随机数
	getRandString(sizeof(requestMsg.r1), requestMsg.r1);

	//生成认证码   requestMsg.authCode
	HMAC_CTX *ctx = NULL;

	//创建一个对应的对象
	ctx = HMAC_CTX_new();
	if (NULL == ctx)
	{
		//写日志
		cout << "HMAC_CTX_new failed..." << endl;
		return 1;
	}

	//初始化
	ret = HMAC_Init_ex(ctx, requestMsg.serverId, strlen(requestMsg.serverId), EVP_sha256(), NULL);
	if (0 == ret)
	{
		cout << "HMAC_Init_ex failed...." << endl;
		return 1;
	}

	//添加数据
	ret = HMAC_Update(ctx, (unsigned char *)requestMsg.r1, strlen(requestMsg.r1));
	if (0 == ret)
	{
		cout << "HMAC_Update failed..." << endl;
		return 1;
	}

	//获取认证码
	ret = HMAC_Final(ctx, (unsigned char *)buf, (unsigned int *)&bufLen);
	if (0 == ret)
	{
		cout << "HMAC_Final failed... " << endl;
		return 1;
	}

	for (int i = 0; i < bufLen; i++)
	{
		sprintf(&requestMsg.authCode[i * 2], "%02X", (unsigned char)buf[i]);	
	}
	cout << "authCode:" << requestMsg.authCode << endl;

	//释放内存
	HMAC_CTX_free(ctx);


	//2. 编码密钥协商请求报文
	RequestFactory reqFactory;	//请求报文的工厂
	Codec *codec = reqFactory.createCodec(&requestMsg);	//创建一个编解码请求报文的对象

	char *outData = NULL;
	int outDataLen = 0;

	codec->msgEncode(&outData, outDataLen); //1. free
	delete codec;
	codec = NULL;

	//3. 客户端连接服务端  TcpSocket
	//TcpSocket tcpSocket;
	ret = m_socket.connectToHost(m_info->serverIP, m_info->serverPort, 0);
	if (0 == ret)
	{
		cout << "连接服务端ok..." << endl;
	}

	//4. 发送编码好之后的数据到服务器
	m_socket.sendMsg(outData, outDataLen, 0);
	delete []outData; //释放内存
	outData = NULL;

	//5. 接收服务端响应报文
	m_socket.recvMsg(&outData, outDataLen, 0); //2. free

	//6. 解码服务端响应报文
	RespondFactory resFactory;  //响应报文编解码工厂
	codec = resFactory.createCodec();
	RespondMsg *respondMsg = (RespondMsg*)codec->msgDecode(outData, outDataLen); //3. free
	free(outData); //释放内存
	outData = NULL;

	//7. 判断服务端响应的rv
	if (0 == respondMsg->rv)
	{
		cout << "密钥协商ok...." << endl;
	}
	else
	{
		cout << "密钥协商失败..." << endl;
		return 1;
	}

	//8. 将密钥信息写入共享内存

	NodeSHMInfo nodeSHMInfo;
	memset(&nodeSHMInfo, 0, sizeof(NodeSHMInfo));
	nodeSHMInfo.status = 0;	//表示密钥状态  0表示有效  1表示无效
	nodeSHMInfo.seckeyID = respondMsg->seckeyid;
	strcpy(nodeSHMInfo.serverID, respondMsg->serverId);
	strcpy(nodeSHMInfo.clientID, respondMsg->clientId);

	//生成密钥信息  nodeSHMInfo.secKey
	//使用SHA512算法模拟生成密钥的信息
	SHA512_CTX shaCtx;
	//初始化
	ret = SHA512_Init(&shaCtx);  
	if (0 == ret)
	{
		cout << "SHA512_Init failed... " << endl;
		return 1;
	}

	//添加数据
	ret = SHA512_Update(&shaCtx, requestMsg.r1, strlen(requestMsg.r1));
	if (0 == ret)
	{
		cout << "SHA512_Update failed... " << endl;
		return 1;
	}

	//添加数据
	ret = SHA512_Update(&shaCtx, respondMsg->r2, strlen((char *)respondMsg->r2));
	if (0 == ret)
	{
		cout << "SHA512_Update failed... " << endl;
		return 1;
	}

	//获取数据信息  
	unsigned char md[SHA512_DIGEST_LENGTH];
	ret = SHA512_Final(md, &shaCtx);
	if (0 == ret)
	{
		cout << "SHA512_Final failed... " << endl;
		return 1;
	}

	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		sprintf(&nodeSHMInfo.secKey[i * 2], "%02X", md[i]);
	}

	//将密钥信息写入共享内存
	m_shm->shmWrite(&nodeSHMInfo);

	delete codec;
	codec = NULL;

	return 0;
}

// 秘钥校验
int ClientOperation::secKeyCheck()
{

}

// 秘钥注销
int ClientOperation::secKeyRevoke()
{

}

// 秘钥查看
int ClientOperation::secKeyView()
{

}


//获取一个随机字符串
void ClientOperation::getRandString(int len, char* randBuf)
{
    int index = -1;

    //A-Z  a-z  0-9 !@#$%^&*+-~
    if (len <= 0 || NULL == randBuf)
    {
        //写日志 
        cout << "参数非法..." << endl;
        return;
    }

    //设置随机种子
    srandom(time(NULL));

    memset(randBuf, 0, len);

    for (int i = 0; i < len - 1; i++)
    {
        //随机任意类型 4类字符
        index = random() % 4;    
    
        switch(index) 
        {
            //大写字母
            case 0:
                randBuf[i] = 'A' + random() % 26;
                break;

            //小写字母
            case 1: 
                randBuf[i] = 'a' + random() % 26;
                break;

            //数字
            case 2:
                randBuf[i] = '0' + random() % 10;
                break;

            //特殊字符
            case 3:
                randBuf[i] = "!@#$%^&*_+-*"[random() % 13];
                break;
        
            default:
                cout << "无效字符类型..." << endl;
        }
    }
}







