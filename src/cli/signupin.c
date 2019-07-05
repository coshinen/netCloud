// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include "signupin.h"

#include "command.h"

#include <stdio.h>
#include <time.h>

extern char bash[256];

void ParseCommandStart(char * cmd)
{
    for (int idx = 0, i = 0; idx != strlen(cmd); ++idx)
    {
        if (cmd[idx] != ' ') {
            cmd[i] = cmd[idx];
            ++i;
            if (cmd[idx + 1] == ' ' || cmd[idx + 1] == '\n') {
                cmd[i] = 0;
                break;
            }
        }
    }
}

void GetSalt(char * salt, int inum)
{
    strcpy(salt, "$6$");
    char str[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;<>?:";
    char tmpSalt[2] = {0};
    int lenStr = strlen(str);;
    srand((unsigned int)time((time_t*)NULL));
    for (int idx = 0; idx != inum - 3; ++idx)
    {
        sprintf(tmpSalt, "%c", str[(rand()%lenStr)]);
        strcat(salt, tmpSalt);
    }
}

int SignUp(int sfd)
{
    char flag = -2;
    int ret;
Label:
    printf("Enter username: ");
    fflush(stdout);
    char username[64] = {0};
    read(0, username, sizeof(username));
    if (!strcmp("\n", username)) {
        goto Label;
    }
    username[strlen(username) - 1] = 0;

    Train_t train;
    bzero(&train, sizeof(Train_t));
    strcpy(train._buf, username);
    train._len = strlen(train._buf);
    ret = sendN(sfd, (char*)&train, sizeof(int) + train._len);
    if (-1 == ret) {
        return -1;
    }
    
    recvN(sfd, &flag, sizeof(char));
    if (-1 == flag) {
        printf("The username already exists! Please enter again.\n");
        goto Label;
    }
    printf("The username can be used!\n");
    
    char * password;
    char passwordVerify[64];
    char * passwordAgain;
LabelAgain:    
    password = getpass("Enter new FTPD password: ");
    bzero(passwordVerify, sizeof(passwordVerify));
    strcpy(passwordVerify, password);
    passwordAgain = getpass("Retype new FTPD password: ");
    if (!strcmp(passwordVerify, passwordAgain)) {
    } else {
        printf("Sorry, passwords do not match\n"
                "Please enter again\n");
        goto LabelAgain;
    }

    char salt[12] = {0};
    GetSalt(salt, 11);

    char * passwd = crypt(password, salt);

    bzero(&train, sizeof(Train_t));
    strcpy(train._buf, passwd);
    train._len = strlen(train._buf);
    ret = sendN(sfd, (char*)&train, sizeof(int) + train._len);
    if (-1 == ret) {
        return -1;
    }

    recvN(sfd, &flag, sizeof(char));
    if (-2 == flag) {
        printf("oops! The server was shutdown.\n");
        return -1;
    } else if (-1 == flag) {
        printf("Sign up failed!\n");
        goto Label;
    } else if (0 == flag) {
        printf("Sign up succeeded!\n");
    }

    return 0;
}

int VerifySignInInfo(int sfd)
{
    char flag;
    int ret;
LabelUsername:
    printf("login as:");
    fflush(stdout);
    char username[64] = {0};
    read(0, username, sizeof(username));
    if (!strcmp("\n", username)) {
        goto LabelUsername;
    }
    username[strlen(username) - 1] = 0;

    Train_t train;
    bzero(&train, sizeof(Train_t));
    strcpy(train._buf, username);    
    train._len = strlen(train._buf);
    ret = sendN(sfd, (char*)&train, sizeof(train._len) + train._len);
    if (-1 == ret) {
        return -1;
    }

    recvN(sfd, &flag, sizeof(char));
    if (-1 == flag) {
        printf("The username does not exist! Please enter again.\n");
        goto LabelUsername;
    }

    char salt[12] = {0};
    recvN(sfd, (char*)&train._len, sizeof(int));
    recvN(sfd, train._buf, train._len);
    strcpy(salt, train._buf);
LabelPassword:
    bzero(&train, sizeof(Train_t));
    char passwordInfo[64] = {0};
    char signinIp[16] = {"127.0.0.1"};
    //getLocalIP(sfd, signinIp);
    sprintf(passwordInfo, "%s%s%s%s%s", username, "@", signinIp, "'s", " password:");
    char * password = getpass(passwordInfo);

    char * passwd = crypt(password, salt);

    strcpy(train._buf, passwd);
    train._len = strlen(train._buf);
    ret = sendN(sfd, (char*)&train, sizeof(train._len) + train._len);
    if (-1 == ret) {
        return -1;
    }

    char signinIpLast[16] = {0};
    char signinDateLast[20] = {0};
    
    recvN(sfd, &flag, sizeof(char));
    if (-1 == flag) {
        printf("Access denied!\n");
        goto LabelPassword;
    } else if (0 == flag) {
        recvN(sfd, signinIpLast, sizeof(signinIpLast));
        recvN(sfd, signinDateLast, sizeof(signinDateLast));
    
        printf("Welcome to netCloud 0.1\n\n"
                " * Documentation:         https://github.cm/mistydew/ftp-SkyDrive\n"
                " * Management & Support:  https://mistydew@qq.com\n\n"
                "Last login: %s from %s\n"
                "Type `help' to learn how to use netCloud prompt.\n", signinDateLast, signinIpLast);
    }

    sprintf(bash, "%s%s%s", username, "@", "ftpd:/");

    return 0;
}
