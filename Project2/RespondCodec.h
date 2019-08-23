#pragma once
#include "Codec.h"


//密钥应答报文
struct  RespondMsg
{
	int					rv;				// 返回值
	char				clientId[12];	// 客户端编号
	char				serverId[12];	// 服务器编号
	unsigned char		r2[64];			// 服务器端随机数
	int					seckeyid;		// 对称密钥编号    keysn
};

class RespondCodec : public Codec
{
public:
	RespondCodec();
	RespondCodec(RespondMsg *msg);
	~RespondCodec();


public: 
	// 重写父类函数
	//编码函数
	int msgEncode(char** outData, int &len);

	//解码函数
	void* msgDecode(char *inData, int inLen);

	//内存释放
	int msgMemFree(void **point);

private:
	RespondMsg *m_msg = NULL;

};

