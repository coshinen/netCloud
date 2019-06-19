 ///
 /// @file    threadHandler.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-10-19 21:14:33
 ///

#include "head.h"

void * threadHandler(void * p)
{
	pNode_t pNode = (pNode_t)p;
puts("I am child thread");
printf("%c\n", pNode->_flagCmd);
	if ('d' == pNode->_flagCmd) {
		getsFile(pNode->_sfdTmp, pNode->_fileName);
	} else if ('u' == pNode->_flagCmd) {
		putsFile(pNode->_sfdTmp, pNode->_fileName);
	}
	pthread_exit(0);
}
