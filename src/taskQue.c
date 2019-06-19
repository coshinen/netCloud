 ///
 /// @file    taskQue.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-14 18:57:55
 ///

#include "taskQue.h"

void taskQueInit(pQue_t pQue)
{
	pQue->_capacity = CAP; // configuration
	pthread_mutex_init(&pQue->_mutex, NULL);
}

void taskQueInsertTail(pQue_t pQue, pNode_t pNew)
{
	if (NULL == pQue->_pHead) {
		pQue->_pHead = pNew;
		pQue->_pTail = pNew;
	} else {
		pQue->_pTail->_pNext = pNew;
		pQue->_pTail = pNew;
	}
	++pQue->_size;
}

void taskQueInsertHead(pQue_t pQue, pNode_t pNew)
{
	if (NULL == pQue->_pHead) {
		pQue->_pHead = pNew;
		pQue->_pTail = pNew;
	} else {
		pNew->_pNext = pQue->_pHead;
		pQue->_pHead = pNew;
	}
	++pQue->_size;
}

void taskQueGet(pQue_t pQue, pNode_t * pCur)
{
	*pCur = pQue->_pHead;
	pQue->_pHead = pQue->_pHead->_pNext;
	if (NULL == pQue->_pHead) {
		pQue->_pTail = NULL;
	}
	--pQue->_size;
}

void taskQueDelete(pNode_t * ppHead, pNode_t * ppTail, pNode_t pNode)
{
	pNode_t pPre = *ppHead;
	pNode_t pCur = *ppHead;
	if (NULL == *ppHead) {
		printf("list is none!\n");
		return;
	} else if (pNode->_sfdNew == (*ppHead)->_sfdNew) {
		*ppHead = (*ppHead)->_pNext;
		free(pCur);
		if (NULL == *ppHead) {
			*ppTail = NULL;
		}
	} else {
		while (pCur != NULL)
		{
			if (pNode->_sfdNew != pCur->_sfdNew) {
				pPre = pCur;
				pCur = pCur->_pNext;
			} else {
				pPre->_pNext = pCur->_pNext;
				free(pCur);
				break;
			}
		}
		if (NULL == pPre->_pNext) {
			*ppTail = pPre;
		}
		if (NULL == pCur) {
			printf("list no this node!\n");
		} else if (pCur != NULL) {
			pCur = NULL;
		}
	}
}

void taskQueModify(pNode_t pHead, int from, int to)
{
	while (pHead != NULL)
	{
		if (from == pHead->_sfdNew) {
			pHead->_sfdNew = to;
			break;
		}
		pHead = pHead->_pNext;
	}
}

void taskQueDestory(pQue_t pQue)
{
	pNode_t pTmp;
	while (pQue->_pHead != NULL)
	{
		pTmp = pQue->_pHead->_pNext;
		free(pQue->_pHead);
		pQue->_pHead = pTmp;
	}
	pQue->_pTail = NULL;
}
