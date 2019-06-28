// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#ifndef __UTIL_H__
#define __UTIL_H__

#include "head.h"

#include <stdbool.h>

typedef struct {
    char pathConf[1024];
    _Bool bDaemon;
    char pathDataDir[1024];
    char sIP[16];
    int nConn;
    unsigned short nPort;
    int nThreads;
} Conf;

Conf mapArgs;

void ParseParameters(int argc, char* argv[]);
void LicenseInfo();
void HelpMessage();
void GetDefaultDataDir(char* path);
void GetDataDir(char* path);
void GetConfigFile(char* path);
void ReadConfigFile(char*);
int InitSocket();

int exitfd[2]; // 异步拉起同步
void setExit(); // 退出机制

#endif
