// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 
#include "threadhandler.h"

#include "factory.h"

void cleanupCondMutex(void * p)
{
    pthread_mutex_t * mutex = (pthread_mutex_t*)p;
    pthread_mutex_unlock(mutex);
}

void cleanup(void * p)
{
    pNode_t pCur = (pNode_t)p;
    close(pCur->_sfdNew);
    free(pCur);
}

void * ThreadHandler(void * p)
{
    pFactory_t pFactory = (pFactory_t)p;
    pQue_t pQue = &pFactory->_queFile;
    
    pNode_t pCur;
    int ret = 0;
    while (1)
    {
        pthread_cleanup_push(cleanupCondMutex, &pQue->_mutex);
        pthread_mutex_lock(&pQue->_mutex);
        if (NULL == pQue->_pHead) {
            pthread_cond_wait(&pFactory->_cond, &pQue->_mutex);
        }
        taskQueGet(pQue, &pCur);
        pthread_mutex_unlock(&pQue->_mutex);
        pthread_cleanup_pop(0);
        
        pthread_cleanup_push(cleanup, pCur);
        
        if ('d' == pCur->_flagCmd) {
            ret = getsFile(pCur, pCur->_fileName);
        } else if ('u' == pCur->_flagCmd) {
            ret = putsFile(pCur, pCur->_fileName);
        }
        free(pCur);
        
        pthread_cleanup_pop(0);
    }
}
