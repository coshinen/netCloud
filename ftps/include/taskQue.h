 ///
 /// @file    workQue.h
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-13 23:32:49
 ///

#ifndef __TASKQUE_H__
#define __TASKQUE_H__

#include "head.h"

typedef struct Node { // 存放网络描述符链式结点
	int _sfdNew;
	char _ip[16];
	unsigned short _port;
	char _user[64]; // 用户
	char _path[256]; // 目录
	size_t _idxLen; // 标识目录尾部的游标/目录长度
	size_t _inum; // 目录层级
	size_t _lenDir[16]; // 每级目录的长度（包含/）
	char _fileName[64];
	char _flagCmd;
	char _flagSigninStatus;
	char _flagSigninProgress;
	char _flagSignupProgress;
	struct Node * _pNext;
} Node_t, * pNode_t;

typedef struct { // 存放网络描述符链式队列
	pNode_t _pHead, _pTail;
	size_t _capacity; // 队列的最大容量
	size_t _size; // 当前队列大小
	pthread_mutex_t _mutex; // 队列锁，保证入队和出队安全
} Que_t, * pQue_t;

#define CAP 1024

void taskQueInit(pQue_t);
void taskQueInsertTail(pQue_t, pNode_t);
void taskQueInsertHead(pQue_t, pNode_t);
void taskQueGet(pQue_t, pNode_t*);
void taskQueDelete(pNode_t*, pNode_t*, pNode_t);
void taskQueModify(pNode_t, int, int);
void taskQueDestory(pQue_t);

#endif
