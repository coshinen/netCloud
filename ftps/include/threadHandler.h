 ///
 /// @file    threadHandler.h
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-16 14:47:44
 ///
 
#ifndef __THREADHANDLER_H__
#define __THREADHANDLER_H__

#include "head.h"
#include "taskQue.h"
#include "factory.h"

void * threadHandler(void*); // 线程行为函数

void getSalt(char*, const char*); // 获取盐值
ssize_t verifyMysqlUserName(const char*, char*, char*, char*, char*, char*);
ssize_t insertMysqlUserInfo(const char*, const char*, const char*, const char*, const char*, const char*, const char*);
ssize_t updateMysqlUserInfo(const char*, const char*, const char*);
ssize_t verifyMysqlFileSystem(size_t*, char*, char*, size_t*, char*, size_t*);
ssize_t insertMysqlFileSystem(const char*, size_t, const char*, const char*, const char*, const char*, size_t, const char*, size_t, const char*, size_t);
ssize_t updateMysqlFileSystem(const char*, const char*, const char*, const size_t*, const char*);
ssize_t deleteMysqlFileSystem(const char*);
ssize_t signUp(pNode_t);
ssize_t verifySignInInfo(pNode_t);

void getCommand(pNode_t);
char ** parseCommand(const char*);
ssize_t selectCommand(char**, pNode_t);

ssize_t sendN(int, const char*, size_t);
ssize_t recvN(int, char*, size_t);
ssize_t getMD5(int, off_t, char*);
void getDate(char*);
size_t float2str(char*, off_t);
void convertSize(char*, double);

void helpFile(pNode_t);
ssize_t listFiles(pNode_t, char**);
void printWorkingDirectory(pNode_t);
size_t queryInum(const char*);
size_t getLenDir(char*);
void changeDirectory(pNode_t, char*);
ssize_t getsFileAgain(pNode_t, int, off_t);
ssize_t getsMappingLargeFile(pNode_t, off_t, int, off_t);
ssize_t getsFile(pNode_t, const char*); // sendData
ssize_t putsMappingLargeFile(pNode_t, off_t, int);
ssize_t putsFile(pNode_t, const char*);
void removeFile(pNode_t, const char*);
void makeDirectory(pNode_t, const char*);
void removeDirectory(pNode_t, const char*);
void renameFile(pNode_t, const char*, const char*);
ssize_t signOut(pNode_t);

#endif
