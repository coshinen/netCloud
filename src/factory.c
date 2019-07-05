// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include "factory.h"

void FactoryInit(pFactory_t pFactory, int numThread, pthreadHandler_t threadHandler)
{
    taskQueInit(&pFactory->_que);
    taskQueInit(&pFactory->_queFile);
    pthread_cond_init(&pFactory->_cond, NULL);
    pFactory->_pThreadHandler= threadHandler;
    pFactory->_pThreadId = (pthread_t*)calloc(numThread, sizeof(pthread_t));
    pFactory->_numThread = numThread;

    if (0 == pFactory->_flagStart)
    {
        for (int idx = 0; idx != pFactory->_numThread; ++idx)
        {
            pthread_create(pFactory->_pThreadId + idx, NULL, pFactory->_pThreadHandler, pFactory);
            printf("pthid = %lu\n", pFactory->_pThreadId[idx]);
        }
        pFactory->_flagStart = 1;
    }
}
