// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 
#ifndef __SIGNUPIN_H__
#define __SIGNUPIN_H__

#include "taskqueue.h"

void GetSalt(char*, const char*);
int SignUp(pNode_t);
int VerifySignInInfo(pNode_t);

#endif
