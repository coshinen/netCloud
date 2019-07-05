// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include "threadhandler.h"

#include "head.h"

void * ThreadHandler(void * p)
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
