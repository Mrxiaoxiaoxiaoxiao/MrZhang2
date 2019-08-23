#define _CRT_SECURE_NO_WARNINGS
#include "SequenceASN1.h"

#include <iostream>

using namespace std;

//构造函数
SequenceASN1::SequenceASN1():BaseASN1()
{

}

//编码第一个节点  
int SequenceASN1::writeHeadNode(int iValue)
{
	int ret = 0;
	ret = DER_ItAsn1_WriteInteger(iValue, &m_header);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteInteger failed..." << endl;
		return ret;
	}

	m_next = m_header;

	return 0;
}

int SequenceASN1::writeHeadNode(char * sValue, int len)
{
	int ret = 0;
	//先将char* 转为ANYBUF类型
	ret = DER_ITCAST_String_To_AnyBuf(&m_temp, (unsigned char *)sValue, len);
	if (0 != ret)
	{
		cout << "DER_ITCAST_String_To_AnyBuf failed..." << endl;
		return ret;
	}

	//编码char*
	ret = DER_ItAsn1_WritePrintableString(m_temp, &m_header);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WritePrintableString failed..." << endl;
		//释放临时节点空间
		DER_ITCAST_FreeQueue(m_temp);
		return ret;
	}

	//释放临时节点空间
	DER_ITCAST_FreeQueue(m_temp);

	m_next = m_header;

	return 0;
}

//添加后继节点
int SequenceASN1::writeNextNode(int iValue)
{
	int ret = 0;

	//编码链表中非头结点节点
	ret = DER_ItAsn1_WriteInteger(iValue, &m_next->next);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteInteger failed..." << endl;
		return ret;
	}

	//m_next指向新节点
	m_next = m_next->next;
	return 0;
}

//添加节点 但不是头结点
int SequenceASN1::writeNextNode(char * sValue, int len)
{
	int ret = 0;
	//先将char* 转为ANYBUF类型
	ret = DER_ITCAST_String_To_AnyBuf(&m_temp, (unsigned char *)sValue, len);
	if (0 != ret)
	{
		cout << "DER_ITCAST_String_To_AnyBuf failed..." << endl;
		return ret;
	}

	//编码char*
	ret = DER_ItAsn1_WritePrintableString(m_temp, &m_next->next);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WritePrintableString failed..." << endl;
		//释放临时节点空间
		DER_ITCAST_FreeQueue(m_temp);
		return ret;
	}

	//释放临时节点空间
	DER_ITCAST_FreeQueue(m_temp);

	//指向链表的最后一个节点
	m_next = m_next->next;

	return 0;
}

//读第一个节点
int SequenceASN1::readHeadNode(int & iValue)
{
	int value;
	int ret = -1;

	//解码
	ret = DER_ItAsn1_ReadInteger(m_header, (ITCAST_UINT32 *)&value);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadInteger failed... " << endl;
		return ret;
	}

	iValue = value;

	//指向下一个节点
	m_next = m_header->next;

	return 0;
}

//读取第一个节点
int SequenceASN1::readHeadNode(char * sValue)
{
	int ret = -1;

	//解码char*数据
	ret = DER_ItAsn1_ReadPrintableString(m_header, &m_temp);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadPrintableString failed.. " << endl;
		return ret;
	}

	//拷贝数据
	memcpy(sValue, m_temp->pData, m_temp->dataLen);

	m_next = m_next->next;

	//释放临时节点空间
	DER_ITCAST_FreeQueue(m_temp);

	return 0;
}

//解码链表其它节点
int SequenceASN1::readNextNode(int & iValue)
{
	int value;
	int ret = -1;

	//解码
	ret = DER_ItAsn1_ReadInteger(m_next, (ITCAST_UINT32 *)&value);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadInteger failed... " << endl;
		return ret;
	}

	iValue = value;
	
	//指向下一个节点
	m_next = m_next->next;

	return 0;
}

//解码链表其它节点
int SequenceASN1::readNextNode(char * sValue)
{
	int ret = -1;

	//解码char*数据
	ret = DER_ItAsn1_ReadPrintableString(m_next, &m_temp);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadPrintableString failed.. " << endl;
		return ret;
	}

	//拷贝数据
	memcpy(sValue, m_temp->pData, m_temp->dataLen);

	m_next = m_next->next;

	//释放临时节点空间
	DER_ITCAST_FreeQueue(m_temp);

	return 0;
}


//编码大结构体
int SequenceASN1::packSequence(char ** outData, int & outLen)
{
	int ret = 0;

	//编码大结构体  
	ret = DER_ItAsn1_WriteSequence(m_header, &m_temp);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteSequence failed..." << endl;
		return ret;
	}

#if 0
	cout << "dataLen: " << m_temp->dataLen << endl;
	cout << "data: " << m_temp->pData << endl;
#endif

	//分配空间
	*outData = new char[m_temp->dataLen + 1];
	if (NULL == *outData)
	{
		//释放临时节点空间
		DER_ITCAST_FreeQueue(m_temp);

		cout << "new outData failed..." << endl;
		return MemoryErr;
	}

	//使用指针问题
	memcpy(*outData, m_temp->pData, m_temp->dataLen);
	outLen = m_temp->dataLen;

	//释放临时节点空间
	DER_ITCAST_FreeQueue(m_temp);

	return 0;
}

//解码大结构体
int SequenceASN1::unpackSequence(char * inData, int inLen)
{
	int ret = 0;

	//1. 将char*转为ANYBUF类型
	ret = DER_ITCAST_String_To_AnyBuf(&m_temp, (unsigned char *)inData, inLen);
	if (0 != ret)
	{
		cout << "DER_ITCAST_String_To_AnyBuf failed..." << endl;
		return ret;
	}

	//2. 解码大结构体
	ret = DER_ItAsn1_ReadSequence(m_temp, &m_header);
	if (0 != ret)
	{
		//3. 释放临时节点空间
		DER_ITCAST_FreeQueue(m_temp);

		cout << "DER_ItAsn1_ReadSequence failed..." << endl;

		return ret;
	}

	//3. 释放临时节点空间
	DER_ITCAST_FreeQueue(m_temp);

	return 0;
}

void SequenceASN1::freeSequence(ITCAST_ANYBUF * node)
{
	int ret = 0;

	//释放链表 m_header
	if (NULL == node)
	{
		DER_ITCAST_FreeQueue(m_header);
	}
	else
	{
		//释放节点
		DER_ITCAST_FreeQueue(node);
	}
}
