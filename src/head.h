// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#ifndef __HEAD_H__
#define __HEAD_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <shadow.h>
#include <crypt.h>
#include <sys/mman.h>
#include <signal.h>
#include <syslog.h>
#include <mysql/mysql.h>
#include <openssl/md5.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <errno.h>

int exitfd[2]; // 异步拉起同步
static char * ROOTPATH = "/tmp/netCloud/";

void LicenseInfo();
void HelpMessage();
char** ReadConfigFile(char**);
void setExit(); // 退出机制
int sblSocket(char**); // 初始化套接字，socket、bind、listen

void createMysqlUserInfo();
void createMysqlFileSystem();

#endif
