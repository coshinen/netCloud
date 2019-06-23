 ///
 /// @file    factory.h
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-14 08:39:12
 ///

#ifndef __FACTORY_H__
#define __FACTORY_H__

#include "head.h"
#include "taskqueue.h"

typedef struct { // 火车模型
    int _len; // 车头，存放车身的长度
    char _buf[1024]; // 车身，存放要传输的数据
} Train_t, * pTrain_t;

typedef void * (*pthreadHandler_t)(void*);

typedef struct {
    Que_t _que, _queFile;
    pthread_cond_t _cond;
    pthreadHandler_t _pThreadHandler;
    pthread_t * _pThreadId;
    int _numThread;
    short _flagStart;
} Factory_t, * pFactory_t;

void factoryInit(pFactory_t, int, pthreadHandler_t);
void factoryStart(pFactory_t);

#endif
