// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <fcntl.h>

typedef struct {
    int _sfd, _sfdTmp;
    char _fileName[64];
    char _flagCmd;
} Node_t, * pNode_t;

typedef struct { // train modles
    int _len; // head: the length of body
    char _buf[1024]; // body: data
} Train_t, * pTrain_t;

static int sfdTmp = -1;

void getCommand(pNode_t, char* argv[]);
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
