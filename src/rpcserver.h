// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 
#ifndef __RPCSERVER_H__
#define __RPCSERVER_H__

#include "taskqueue.h"
#include "factory.h"

#include <sys/types.h>

int GetCommand(pNode_t, pFactory_t);
char ** parseCommand(const char*);
int selectCommand(char**, pNode_t, pFactory_t);

int sendN(int, const char*, int);
int recvN(int, char*, int);
int getMD5(int, off_t, char*);
void getDate(char*);
int float2str(char*, off_t);
void convertSize(char*, double);

void helpFile(pNode_t);
int listFiles(pNode_t, char**);
void printWorkingDirectory(pNode_t);
int queryInum(const char*);
int getLenDir(char*);
void changeDirectory(pNode_t, char*);
int getsFileAgain(pNode_t, int, off_t);
int getsMappingLargeFile(pNode_t, off_t, int, off_t);
int getsFile(pNode_t, const char*); // sendData
int putsMappingLargeFile(pNode_t, off_t, int);
int putsFile(pNode_t, const char*);
void removeFile(pNode_t, const char*);
void makeDirectory(pNode_t, const char*);
void removeDirectory(pNode_t, const char*);
void renameFile(pNode_t, const char*, const char*);
int signOut(pNode_t);

#endif
