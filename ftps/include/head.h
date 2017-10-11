 ///
 /// @file    head.h
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-13 15:05:00
 ///

 // 命名规范：
 // 变量名：变量小写 + 修饰首字母大写
 // 函数：动词小写 + 名词首字母大写
 // RETURN为-1表示错误，0表示正常结束，1表示开始标志 

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

int exitfd[2]; // 异步拉起同步

char ** readConf(char**); // 读启动配置文件
void getDaemon(); // 变身守护进程
void setExit(); // 退出机制
int sblSocket(char**); // 初始化套接字，socket、bind、listen
void createMysqlUserInfo();
void createMysqlFileSystem();

#endif
