 ///
 /// @file    signupin.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-10-19 18:05:20
 ///
 
#include "threadhandler.h"

void getSalt(char * salt, const char * passwd)
{
    size_t idx, i$;
    for (idx = 0, i$ = 0; i$ != 3 && idx != strlen(passwd); ++idx)
    {
        if (passwd[idx] == '$') {
            ++i$;
        }
    }
    strncpy(salt, passwd, idx - 1);
}

ssize_t signUp(pNode_t pNode)
{
    ssize_t ret;
    char flag;
    Train_t train;
    if (0 == pNode->_flagSignupProgress) {
        flag = 0;
        bzero(&train, sizeof(Train_t));
        ret = recvN(pNode->_sfdNew, (char*)&train._len, sizeof(size_t));
        if (-1 == ret || 0 == ret) {
            return -1;
        }
        ret = recvN(pNode->_sfdNew, train._buf, train._len);
        if (-1 == ret || 0 == ret) {
            return -1;
        }
    
        char username[64] = {0};
        strcpy(username, train._buf);
    
        ret = verifyMysqlUserName(username, NULL, NULL, NULL, NULL, NULL);
        if (0 == ret) {
            flag = -1;
            sendN(pNode->_sfdNew, &flag, sizeof(char));
            return 0;
        }
        sendN(pNode->_sfdNew, &flag, sizeof(char));

        bzero(pNode->_user, sizeof(pNode->_user));
        strcpy(pNode->_user, username);
        pNode->_flagSignupProgress = 1;
    } else if (1 == pNode->_flagSignupProgress) {
        bzero(&train, sizeof(Train_t));
        ret = recvN(pNode->_sfdNew, (char*)&train._len, sizeof(size_t));
        if (-1 == ret || 0 == ret) {
            return -1;
        }
        ret = recvN(pNode->_sfdNew, train._buf, train._len);
        if (-1 == ret || 0 == ret) {
            return -1;
        }
    
        char passwd[99] = {0};
        strcpy(passwd, train._buf);
    
        char salt[12] = {0};
        getSalt(salt, passwd);
    
        char signupDate[20] = {0};
        time_t t;
        time(&t);
        struct tm * pgm = gmtime(&t);
        sprintf(signupDate, "%d-%02d-%02d %02d:%02d:%02d", 1900 + pgm->tm_year, 1 + pgm->tm_mon, pgm->tm_mday, 8 + pgm->tm_hour, pgm->tm_min, pgm->tm_sec);
    
        ret = insertMysqlUserInfo(pNode->_user, passwd, salt, pNode->_ip, signupDate, pNode->_ip, signupDate);
        if (-1 == ret) {
            flag = -1;
            sendN(pNode->_sfdNew, &flag, sizeof(char));
        } else if (0 == ret) {
            flag = 0;
            sendN(pNode->_sfdNew, &flag, sizeof(char));
            
            pNode->_flagSigninStatus = 0;
        }
        pNode->_flagSignupProgress = 0;
    }
    return 0;
}

ssize_t verifySignInInfo(pNode_t pNode)
{
    char flag;
    ssize_t ret;
    Train_t train;
    char username[64] = {0};
    char salt[12] = {0}; // 盐值11位
    char passwd[99] = {0}; // 加密密码98位
    if (0 == pNode->_flagSigninProgress) {
        flag = 0;
        bzero(&train, sizeof(Train_t));
        ret = recvN(pNode->_sfdNew, (char*)&train._len, sizeof(train._len));
        if (-1 == ret || 0 == ret) {
            return -1;
        }
        ret = recvN(pNode->_sfdNew, train._buf, train._len);
        if (-1 == ret || 0 == ret) {
            return -1;
        }
        strcpy(username, train._buf);
    
        ret = verifyMysqlUserName(username, NULL, salt, NULL, NULL, NULL);
        if (-1 == ret) {
            flag = -1;
            sendN(pNode->_sfdNew, &flag, sizeof(char));
            return 0;
        }
        sendN(pNode->_sfdNew, &flag, sizeof(char));
    
        bzero(&train, sizeof(Train_t));
        strcpy(train._buf, salt);
        train._len = strlen(train._buf);
        sendN(pNode->_sfdNew, (char*)&train, sizeof(size_t) + train._len);

        bzero(pNode->_user, sizeof(pNode->_user));
        strcpy(pNode->_user, username);
        pNode->_flagSigninProgress = 1;
    } else if (1 == pNode->_flagSigninProgress) {
        verifyMysqlUserName(pNode->_user, passwd, NULL, NULL, NULL, NULL);
        char password[99] = {0};
        bzero(&train, sizeof(Train_t));
        ret = recvN(pNode->_sfdNew, (char*)&train._len, sizeof(train._len));
        if (-1 == ret || 0 == ret) {
            return -1;
        }
        ret = recvN(pNode->_sfdNew, train._buf, train._len);
        if (-1 == ret || 0 == ret) {
            return -1;
        }
        strcpy(password, train._buf);
    
        if (!strcmp(passwd, password)) {
            char signupDate[20] = {0};
            char signinIp[16] = {0};
            char signinDate[20] = {0};
            ret = verifyMysqlUserName(pNode->_user, NULL, NULL, signupDate, signinIp, signinDate);
            if (-1 == ret) {
                flag = -1;
                sendN(pNode->_sfdNew, &flag, sizeof(char));
                printf("Verification failed!\n");
                return 0;
            } else if (0 == ret) {
                if (!strcmp(signupDate, signinDate)) {
                    char pathName[256] = {0};
                    sprintf(pathName, "%s%s", pNode->_path, pNode->_user);
                    char date[20] = {0};
                    getDate(date);
                    insertMysqlFileSystem(pNode->_user, 0, "dir", pNode->_user, pNode->_path, pathName, 0, NULL, 4096, date, 4096);
                }
                
                flag = 0;
                sendN(pNode->_sfdNew, &flag, sizeof(char));
        
                sendN(pNode->_sfdNew, signinIp, sizeof(signinIp));
                sendN(pNode->_sfdNew, signinDate, sizeof(signinDate));
                
                time_t t;
                time(&t);
                struct tm * pgm = gmtime(&t);
                sprintf(signinDate, "%d-%02d-%02d %02d:%02d:%02d", 1900 + pgm->tm_year, 1 + pgm->tm_mon, pgm->tm_mday, 8 + pgm->tm_hour, pgm->tm_min, pgm->tm_sec);
                ret = updateMysqlUserInfo(pNode->_user, pNode->_ip, signinDate);
                if (-1 == ret) {
                    flag = -1;
                
                    sendN(pNode->_sfdNew, &flag, sizeof(char));
                    printf("Verification failed!\n");
                    return 0;
                }
    
                syslog(LOG_INFO, "[user] %s [info] %s\n", pNode->_user, "Sign in");
    
                sprintf(pNode->_path + pNode->_idxLen, "%s%s", pNode->_user, "/");
                pNode->_idxLen = strlen(pNode->_path);
                pNode->_lenDir[pNode->_inum] = strlen(pNode->_path);
    
                printf("Verification passed!\n");
                pNode->_flagSigninStatus = 7;
            }
        } else {
            flag = -1;
            
            sendN(pNode->_sfdNew, &flag, sizeof(char));
            printf("Verification failed!\n");
            return 0;
        }
    }
    return 0;
}
