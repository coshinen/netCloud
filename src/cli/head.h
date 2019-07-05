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

typedef struct { // train modles
    int _len; // head: the length of body
    char _buf[1024]; // body: data
} Train_t, * pTrain_t;

#endif
