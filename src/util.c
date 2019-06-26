// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include "head.h"

void LicenseInfo()
{
    char versionInfo[] = {"netCloud Core Daemon version v0.0.1.0\n"};
    fprintf(stdout, "%s", versionInfo);
}

void HelpMessage()
{
    char strUsage[] = {"Usage:\n\n"
                       "  ftpd [options]       Start netCloud Core Daemon\n\n"
                       "Options:\n\n"
                       "  -? / -h / -help\n"
                       "       This help message\n\n"
                       "  -version\n"
                       "       Print version and exit\n\n"
                       "  -port\n"
                       "       Set socket port\n\n"
                       "  -threads\n"
                       "       Set thread number\n"};
    fprintf(stdout, "%s", strUsage);
}

char** ReadConfigFile(char* argv[])
{
    int fd = open(argv[1], O_RDONLY);

    char buf[1024] = {0};
    read(fd, buf, sizeof(char) * 1024);

    char ** argvConf = (char**)calloc(3, sizeof(char*));
    for (int idx = 0; idx != 3; ++idx)
    {
        argvConf[idx] = (char*)calloc(1, sizeof(char) * 16);
    }

    for (int idx = 0, y =0; idx != strlen(buf); ++idx)
    {
        if (buf[idx] == '=')
        {
            ++idx;
            for (int x = 0; buf[idx] != '\n'; ++idx, ++x)
            {
                argvConf[y][x] = buf[idx];
            }
            ++y;
        }
    }

    close(fd);
    return argvConf;
}

void sigHandler(int signum)
{
    char flag = 1;
    write(exitfd[1], &flag, sizeof(char));
}

void setExit()
{
    pipe(exitfd);
    if (fork()) {
        close(exitfd[0]);
        signal(SIGINT, sigHandler);
        wait(NULL);
        exit(0);
    }
    close(exitfd[1]);
    setsid();
    signal(SIGPIPE, SIG_IGN);
}

int sblSocket(char ** argv)
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    
    int reuse = 1; // 设置端口重用
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&reuse, sizeof(reuse));
    
    struct sockaddr_in ser;
    bzero(&ser, sizeof(struct sockaddr_in));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(atoi(argv[1]));
    ser.sin_addr.s_addr = inet_addr(argv[0]);
    bind(sfd, (struct sockaddr*)&ser, sizeof(ser));
    
    listen(sfd, 20);
    
    return sfd;
}
