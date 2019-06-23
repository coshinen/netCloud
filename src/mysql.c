 ///
 /// @file    mysql.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-10-19 17:46:44
 ///
 
#include "threadhandler.h"

void createMysqlUserInfo()
{
    MYSQL mysql;
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
        return;
    } else {
        printf("Connected...\n");
    }
    char query[1024] = {0};
    strcpy(query, "create table UserInfo(UID smallint unsigned primary key AUTO_INCREMENT, USERNAME char(64) not NULL unique, PASSWD char(98) not NULL, SALT char(11) not NULL, SIGNUPIP char(16) not NULL, SIGNUPDATE datetime not NULL, SIGNINIP char(16) not NULL, SIGNINDATE datetime not NULL)");
    if (mysql_query(&mysql, query)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
        return;
    } else {
        printf("create table UserInfo succeeded\n");
    }
    mysql_close(&mysql);
}

void createMysqlFileSystem()
{
    MYSQL mysql;
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
        return;
    } else {
        printf("Connected...\n");
    }
    char query[1024] = {0};
    strcpy(query, "create table FileSystem(USERNAME char(64) not NULL, PREINODE mediumint unsigned not NULL, CURINODE mediumint unsigned not NULL unique AUTO_INCREMENT, FILETYPE char(4) not NULL, FILENAME char(64) not NULL, PATH char(255) not NULL, PATHNAME char(255) not NULL primary key, LINKNUMS mediumint unsigned not NULL, MD5 char(32), FILESIZE bigint unsigned not NULL, DATE datetime not NULL, CURSIZE bigint unsigned not NULL)");
    if (mysql_query(&mysql, query)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
        return;
    } else {
        printf("create table FileSystem succeeded\n");
    }
    mysql_close(&mysql);
}

int verifyMysqlUserName(const char * userName, char * passwd, char * salt, char * signupDate, char * signinIp, char * signinDate)
{
    int flag = -1;
    MYSQL mysql;
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("Connected...\n");
    }
    if (mysql_query(&mysql, "select * from UserInfo")) {
        fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
    } else {
        printf("Query made...\n");
        MYSQL_RES * res = mysql_use_result(&mysql);
        MYSQL_ROW row;
        if (res) {
            while ((row = mysql_fetch_row(res)) != NULL)
            {
                if (!strcmp(row[1], userName)) {
                    flag = 0;
                    if (passwd != NULL) {
                        strcpy(passwd, row[2]);
                    }
                    if (salt != NULL) {
                        strcpy(salt, row[3]);
                    }
                    if (signupDate != NULL) {
                        strcpy(signupDate, row[5]);
                    }
                    if (signinIp != NULL && signinDate != NULL) {
                        strcpy(signinIp, row[6]);
                        strcpy(signinDate, row[7]);
                    }
                    break;
                }
            }
        }
        mysql_free_result(res);
    }
    mysql_close(&mysql);
    
    return flag;
}

int insertMysqlUserInfo(const char * userName, const char * passwd, const char * salt, const char * signupIp, const char * signupDate, const char * signinIp, const char * signinDate)
{
    MYSQL mysql;
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("Connected...\n");
    }
    char query[1024] = {0};
    sprintf(query, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", "insert into UserInfo(USERNAME, PASSWD, SALT, SIGNUPIP, SIGNUPDATE, SIGNINIP, SIGNINDATE) values('", userName, "', '", passwd, "', '", salt, "', '", signupIp, "', '", signupDate, "', '", signinIp, "', '", signinDate, "')");
    puts(query);
    if (mysql_query(&mysql, query)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("insert succeeded\n");
    }
    mysql_close(&mysql);
    
    return 0;
}

int updateMysqlUserInfo(const char * userName, const char * signinIp, const char * signinDate)
{
    MYSQL mysql;
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("Connected...\n");
    }
    char query[256] = {0};
    sprintf(query, "%s%s%s%s%s", "update UserInfo set SIGNINIP = '", signinIp, "' where USERNAME = '", userName, "'");
    puts(query);
    if (mysql_query(&mysql, query)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("update succeeded\n");
    }
    bzero(query, sizeof(query));
    sprintf(query, "%s%s%s%s%s", "update UserInfo set SIGNINDATE = '", signinDate, "' where USERNAME = '", userName, "'");
    puts(query);
    if (mysql_query(&mysql, query)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("update succeeded\n");
    }
    mysql_close(&mysql);

    return 0;
}

int verifyMysqlFileSystem(int * curInode, char * fileType, char * pathName, int * linkNums, char * md5, int * fileSize)
{
    int flag = -1;
    MYSQL mysql;
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("Connected...\n");
    }
    if (mysql_query(&mysql, "select * from FileSystem")) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("Query made...\n");
        MYSQL_RES * res = mysql_use_result(&mysql);
        MYSQL_ROW row;
        if (res) {
            while ((row = mysql_fetch_row(res)) != NULL)
            {
                if (pathName != NULL) {
                    if (curInode != NULL && !strcmp(row[6], pathName)) {
                        *curInode = (int)atol(row[2]);
                    }
                    if (fileType != NULL && !strcmp(row[6], pathName)) {
                        strcpy(fileType, row[3]);
                    }
                    if (linkNums != NULL && !strcmp(row[6], pathName)) {
                        *linkNums = (int)atol(row[7]);
                    }
                    if (md5 != NULL && !strcmp(row[6], pathName)) {
                        strcpy(md5, row[8]);
                    }
                    if (fileSize != NULL && !strcmp(row[6], pathName)) {
                        *fileSize = (int)atol(row[9]);
                    }
                    if (!strcmp(row[6], pathName)) {
                        flag = 0;
                        break;
                    }
                }
                if (md5 != NULL && !strcmp(row[8], md5) && fileSize != NULL && linkNums != NULL) {
                    *linkNums = (int)atol(row[7]);
                    *fileSize = (int)atol(row[9]);
                    
                    flag = 0;
                    break;
                }
            }
        }
        mysql_free_result(res);
    }
    mysql_close(&mysql);
    
    return flag;
}

int insertMysqlFileSystem(const char * userName, int preInode, const char * fileType, const char * fileName, const char * path, const char * pathName, int linkNums, const char * md5, int fileSize, const char * date, int curSize)
{
    MYSQL mysql;
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("Connected...\n");
    }
    char query[1024] = {0};
    sprintf(query, "%s%s%s%lu%s%s%s%s%s%s%s%s%s%lu%s%s%s%lu%s%s%s%lu%s", "insert into FileSystem(USERNAME, PREINODE, FILETYPE, FILENAME, PATH, PATHNAME, LINKNUMS, MD5, FILESIZE, DATE, CURSIZE) values('", userName, "', ", preInode, ", '", fileType, "', '", fileName, "', '", path, "', '", pathName, "',", linkNums, ", '", md5, "', ", fileSize, ", '", date, "', ", curSize, ")");
    puts(query);
    if (mysql_query(&mysql, query)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("insert succeeded\n");
    }
    mysql_close(&mysql);

    return 0;
}

int updateMysqlFileSystem(const char * newFileName, const char * newPathName, const char * oldPathName, const int * linkNums, const char * md5)
{
    MYSQL mysql;
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("Connected...\n");
    }
    char query[256];
    if (oldPathName != NULL && newFileName != NULL) {
        bzero(query, sizeof(query));
        sprintf(query, "%s%s%s%s%s", "update FileSystem set FILENAME = '", newFileName, "' where PATHNAME = '", oldPathName, "'");
        puts(query);
        if (mysql_query(&mysql, query)) {
            mysql_close(&mysql);
            fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
            return -1;
        } else {
            printf("update succeeded\n");
        }
    }
    if (oldPathName != NULL && newPathName != NULL) {
        bzero(query, sizeof(query));
        sprintf(query, "%s%s%s%s%s", "update FileSystem set PATHNAME = '", newPathName, "' where PATHNAME = '", oldPathName, "'");
        puts(query);
        if (mysql_query(&mysql, query)) {
            mysql_close(&mysql);
            fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
            return -1;
        } else {
            printf("update succeeded\n");
        }
    }
    if (oldPathName != NULL && linkNums != NULL) {
        bzero(query, sizeof(query));
        sprintf(query, "%s%lu%s%s%s", "update FileSystem set LINKNUMS = ", *linkNums, " where PATHNAME = '", oldPathName, "'");
        puts(query);
        if (mysql_query(&mysql, query)) {
            mysql_close(&mysql);
            fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
            return -1;
        } else {
            printf("update succeeded\n");
        }
    }
    if (md5 != NULL && linkNums != NULL) {
        bzero(query, sizeof(query));
        sprintf(query, "%s%lu%s%s%s", "update FileSystem set LINKNUMS = ", *linkNums, " where MD5 = '", md5, "'");
        puts(query);
        if (mysql_query(&mysql, query)) {
            mysql_close(&mysql);
            fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
            return -1;
        } else {
            printf("update succeeded\n");
        }
    }
    mysql_close(&mysql);

    return 0;
}

int deleteMysqlFileSystem(const char * pathName)
{
    MYSQL mysql;
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("Connected...\n");
    }
    char query[256] = {0};
    sprintf(query, "%s%s%s", "delete from FileSystem where PATHNAME = '", pathName, "'");
    puts(query);
    if (mysql_query(&mysql, query)) {
        mysql_close(&mysql);
        fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
        return -1;
    } else {
        printf("delete succeeded\n");
    }
    mysql_close(&mysql);

    return 0;
}
