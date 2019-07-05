// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#ifndef __FACTORY_H__
#define __FACTORY_H__

#include "taskqueue.h"

typedef struct { // train modles
    int _len; // head: the length of body
    char _buf[1024]; // body: data
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

void FactoryInit(pFactory_t, int, pthreadHandler_t);

#endif
