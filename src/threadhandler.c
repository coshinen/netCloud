 ///
 /// @file    threadhandler.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-14 21:20:12
 ///
 
#include "threadhandler.h"

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

void * threadHandler(void * p)
{
    pFactory_t pFactory = (pFactory_t)p;
    pQue_t pQue = &pFactory->_queFile;
    
    pNode_t pCur;
    ssize_t ret = 0;
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
