 ///
 /// @file    factory.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-14 14:45:24
 ///

#include "factory.h"

void factoryInit(pFactory_t pFactory, size_t numThread, pthreadHandler_t threadHandler)
{
	taskQueInit(&pFactory->_que);
	taskQueInit(&pFactory->_queFile);
	pthread_cond_init(&pFactory->_cond, NULL);
	pFactory->_pThreadHandler= threadHandler;
	pFactory->_pThreadId = (pthread_t*)calloc(numThread, sizeof(pthread_t));
	pFactory->_numThread = numThread;
}

void factoryStart(pFactory_t pFactory)
{
	if (0 == pFactory->_flagStart) {
		for (size_t idx = 0; idx != pFactory->_numThread; ++idx)
		{
			pthread_create(pFactory->_pThreadId + idx, NULL, pFactory->_pThreadHandler, pFactory);
			printf("pthid = %ld\n", pFactory->_pThreadId[idx]);
		}
		pFactory->_flagStart = 1;
	}
}
