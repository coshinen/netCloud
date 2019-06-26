// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#ifndef __HEAD_H__
#define __HEAD_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <time.h>
#include <shadow.h>
#include <crypt.h>
#include <sys/mman.h>
#include <dirent.h>
#include <signal.h>
#include <openssl/md5.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <pthread.h>

extern char bash[256]; // extern作用域全局
static int idxBash = 0;
static int FLAG = 0; // static作用域在单个.c文件中
static int sfdTmp = -1;

typedef struct {
    int _sfd, _sfdTmp;
    char _fileName[64];
    char _flagCmd;
} Node_t, * pNode_t;

typedef struct { // 火车模型
    int _len; // 车头，存放车身的长度
    char _buf[1024]; // 车身，存放要传输的数据
} Train_t, * pTrain_t;

int scSocket(char**); // 套接字封装
//int getLocalIP(int, char*); // 获取本地IP
char ** readDownloadingConf(const char*); // 读下载配置文件

void * threadHandler(void*);

void parseCommandStart(char*);
void getSalt(char*, int);
int signUp(int);
int verifySignInInfo(int); // 验证登录信息

void getCommand(pNode_t, char**);
char ** parseCommand(const char*);
int verifyCommand(char**); // 参数在服务器端验证
int selectCommand(char**, pNode_t);

int sendN(int, const char*, int);
int recvN(int, char*, int);
int getMD5(int, off_t, char*);
int int2str(char*, off_t);
int float2str(char*, off_t);
void convertSize(char*, double);
int float2strSpeed(char*, off_t);
void convertSizeSpeed(char*, double);

void helpFile(int);
void print(int);
void listFiles(int, char**);
void printWorkingDirectory(int);
void changeDirectory(int, const char*);
void writeTempConf(int, const char*, const char*, const char*);
int getsFileAgain(int, int, const char*, off_t, int, const char*, const char*, char*, const char*);
int getsMappingLargeFile(int, off_t, int, const char*, off_t, int, const char*, const char*, char*);
int getsFile(int, const char*); // recvData
int putsMappingLargeFile(int, off_t, int);
int putsFile(int, const char*);
void removeFile(int, char*);
void makeDirectory(int, char*);
void removeDirectory(int, char*);
void renameFile(int, char*);
int signOut(int);

#endif
