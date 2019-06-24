 ///
 /// @file    preparation.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-16 16:18:22
 ///

#include "head.h"

int scSocket(char ** argv)
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sfd) {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }
    
    struct sockaddr_in ser;
    bzero(&ser, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(atoi(argv[2]));
    ser.sin_addr.s_addr = inet_addr(argv[1]);
    int ret;
Label:
    ret = connect(sfd, (struct sockaddr*)&ser, sizeof(ser));
    if (-1 == ret) {
        goto Label;
    }

    return sfd;
}
#if 0
int getLocalIP(int sfd, char * ip)
{
    struct ifreq ifr;
    bzero(&ifr, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, "ens33", sizeof(ifr.ifr_name));

    if (ioctl(sfd, SIOCGIFADDR, &ifr) < 0) {
        printf("ioctl error: %s\n", strerror(errno));
        close(sfd);
        return -1;
    }

    struct sockaddr_in sin;
    memcpy(&sin, &ifr.ifr_addr, sizeof(struct sockaddr_in));
    strcpy(ip, inet_ntoa(sin.sin_addr));

    return 0;
}
#endif
char ** readDownloadingConf(const char * args)
{
    int fd = open(args, O_RDONLY);
    
    char ** argsConf = (char**)calloc(3,sizeof(char*));
    int idx;
    for (idx = 0; idx != 3; ++idx)
    {
        argsConf[idx] = (char*)calloc(1,sizeof(char) * 16);
    }
    
    char buf[1024] = {0};
    read(fd, buf, sizeof(char) * 1024);
    
    for (int iConf = 0, jConf = 0, iBuf = 2; iBuf != strlen(buf); ++iBuf)
    {
        if (buf[iBuf - 2] == ' ' && buf[iBuf - 1] == '"') {
            for (; iBuf != strlen(buf); ++iBuf)
            {
                if (buf[iBuf] == '"' && (buf[iBuf + 1] == ',' || buf[iBuf + 1] == '\n')) {
                    jConf = 0;
                    ++iConf;
                    break;
                } else {
                    argsConf[iConf][jConf] = buf[iBuf];
                    ++jConf;
                }
            }
        }

        if (3 == iConf) {
            break;
        }
    }
    
    close(fd);
    return argsConf;
}
