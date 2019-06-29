// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
 
#ifndef __MYSQL_H__
#define __MYSQL_H__

#include "head.h"

void createMysqlUserInfo();
void createMysqlFileSystem();
int verifyMysqlUserName(const char*, char*, char*, char*, char*, char*);
int insertMysqlUserInfo(const char*, const char*, const char*, const char*, const char*, const char*, const char*);
int updateMysqlUserInfo(const char*, const char*, const char*);
int verifyMysqlFileSystem(int*, char*, char*, int*, char*, int*);
int insertMysqlFileSystem(const char*, int, const char*, const char*, const char*, const char*, int, const char*, int, const char*, int);
int updateMysqlFileSystem(const char*, const char*, const char*, const int*, const char*);
int deleteMysqlFileSystem(const char*);

#endif
