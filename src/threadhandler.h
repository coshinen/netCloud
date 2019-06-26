// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 
#ifndef __THREADHANDLER_H__
#define __THREADHANDLER_H__

#include "head.h"
#include "taskqueue.h"
#include "factory.h"

void * threadHandler(void*); // 线程行为函数

int verifyMysqlUserName(const char*, char*, char*, char*, char*, char*);
int insertMysqlUserInfo(const char*, const char*, const char*, const char*, const char*, const char*, const char*);
int updateMysqlUserInfo(const char*, const char*, const char*);
int verifyMysqlFileSystem(int*, char*, char*, int*, char*, int*);
int insertMysqlFileSystem(const char*, int, const char*, const char*, const char*, const char*, int, const char*, int, const char*, int);
int updateMysqlFileSystem(const char*, const char*, const char*, const int*, const char*);
int deleteMysqlFileSystem(const char*);

void getSalt(char*, const char*); // 获取盐值
int signUp(pNode_t);
int verifySignInInfo(pNode_t);

int getCommand(pNode_t, pFactory_t);
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
