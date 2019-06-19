 ///
 /// @file    head.h
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-14 22:34:55
 ///

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
static size_t idxBash = 0;
static ssize_t FLAG = 0; // static作用域在单个.c文件中
static int sfdTmp = -1;

typedef struct {
    int _sfd, _sfdTmp;
    char _fileName[64];
    char _flagCmd;
} Node_t, * pNode_t;

typedef struct { // 火车模型
    size_t _len; // 车头，存放车身的长度
    char _buf[1024]; // 车身，存放要传输的数据
} Train_t, * pTrain_t;

int scSocket(char**); // 套接字封装
ssize_t getLocalIP(int, char*); // 获取本地IP
char ** readDownloadingConf(const char*); // 读下载配置文件

void * threadHandler(void*);

void parseCommandStart(char*);
void getSalt(char*, size_t);
ssize_t signUp(int);
ssize_t verifySignInInfo(int); // 验证登录信息

void getCommand(pNode_t, char**);
char ** parseCommand(const char*);
ssize_t verifyCommand(char**); // 参数在服务器端验证
ssize_t selectCommand(char**, pNode_t);

ssize_t sendN(int, const char*, size_t);
ssize_t recvN(int, char*, size_t);
ssize_t getMD5(int, off_t, char*);
size_t int2str(char*, off_t);
size_t float2str(char*, off_t);
void convertSize(char*, double);
size_t float2strSpeed(char*, off_t);
void convertSizeSpeed(char*, double);

void helpFile(int);
void print(int);
void listFiles(int, char**);
void printWorkingDirectory(int);
void changeDirectory(int, const char*);
void writeTempConf(int, const char*, const char*, const char*);
ssize_t getsFileAgain(int, int, const char*, off_t, int, const char*, const char*, char*, const char*);
ssize_t getsMappingLargeFile(int, off_t, int, const char*, off_t, int, const char*, const char*, char*);
ssize_t getsFile(int, const char*); // recvData
ssize_t putsMappingLargeFile(int, off_t, int);
ssize_t putsFile(int, const char*);
void removeFile(int, char*);
void makeDirectory(int, char*);
void removeDirectory(int, char*);
void renameFile(int, char*);
ssize_t signOut(int);

#endif
