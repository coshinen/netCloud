 ///
 /// @file    taskQue.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-14 18:57:55
 ///

#include "../include/taskQue.h"

void taskQueInit(pQue_t pQue)
{
	pQue->_capacity = CAP;
	pthread_mutex_init(&pQue->_mutex, NULL);
}

void taskQueInsertTail(pQue_t pQue, pNode_t pNew)
{
	if(NULL == pQue->_pHead){
		pQue->_pHead = pNew;
		pQue->_pTail = pNew;
	}else{
		pQue->_pTail->_pNext = pNew;
		pQue->_pTail = pNew;
	}
	++pQue->_size;
}

void taskQueGet(pQue_t pQue, pNode_t * pCur)
{
	*pCur = pQue->_pHead;
	pQue->_pHead = pQue->_pHead->_pNext;
	if(NULL == pQue->_pHead){
		pQue->_pTail = NULL;
	}
	--pQue->_size;
}

void taskQueDestory(pQue_t pQue)
{
	pNode_t pTemp;
	while(pQue->_pHead != NULL)
	{
		pTemp = pQue->_pHead;
		pQue->_pHead = pQue->_pHead->_pNext;
		free(pTemp);
	}
	pQue->_pTail = NULL;
}
