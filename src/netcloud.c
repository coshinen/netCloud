 ///
 /// @file    netcloud.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-13 11:11:08
 ///

#include "threadhandler.h"

int AppInit(int argc, char* argv[])
{
    // Parameters: process help and version before taking care about datadir
    if (!strcmp(argv[1], "-?") || !strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "-version"))
    {
        if (!strcmp(argv[1], "-version"))
        {
            LicenseInfo();
        }
        else
        {
            HelpMessage();
        }

        return -1;
    }

    openlog(0, LOG_CONS | LOG_PID, LOG_LOCAL0);

    char ** argvConf = ReadConfigFile(argv);

    // Daemonize
    fprintf(stdout, "netCloud server starting\n");
    pid_t pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "Error: fork() returned %d errno %d\n", pid, errno);
        return -1;
    }
    if (pid > 0) // Parent process, pid is child process id
    {
        return 0;
    }
    // Child process falls through to rest of initialization

    pid_t sid = setsid();
    if (sid < 0)
        fprintf(stderr, "Error: setsid() returned %d errno %d\n", sid, errno);

    umask(0);

    mkdir(ROOTPATH, 0775);
    chdir(ROOTPATH);
#if 0
    for (int idx = 0; idx != 3; ++idx)
    {
        close(idx);
    }
#endif
    int sfd = sblSocket(argvConf);

    createMysqlUserInfo();
    createMysqlFileSystem();

    Factory_t factory;
    bzero(&factory, sizeof(Factory_t));
    int numThread = atol(argvConf[2]);
    factoryInit(&factory, numThread, threadHandler);
    factoryStart(&factory);

    int epfd = epoll_create(1);
    struct epoll_event ev, evs[2 + numThread];
    bzero(&ev, sizeof(struct epoll_event));
    ev.events = EPOLLIN;
    ev.data.fd = sfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev);
    bzero(&ev, sizeof(struct epoll_event));
    ev.events = EPOLLIN;
    ev.data.fd = exitfd[0];
    epoll_ctl(epfd, EPOLL_CTL_ADD, exitfd[0], &ev);

    int retEp;
    int idx;
    int sfdNew;
    struct sockaddr_in cli;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    pNode_t pNode;
    pNode_t pTemp, pCur;
    char flag;
    int ret;
    while (1)
    {
        pTemp = factory._que._pHead;
    
        bzero(evs, sizeof(evs));
        retEp = epoll_wait(epfd, evs, 2 + numThread, -1);
        
        for (idx = 0; idx < retEp; ++idx)
        {
            if (sfd == evs[idx].data.fd) {
                bzero(&cli, sizeof(struct sockaddr_in));
                sfdNew = accept(sfd, (struct sockaddr*)&cli, &addrlen);
                
                bzero(&ev, sizeof(struct epoll_event));
                ev.events = EPOLLIN;
                ev.data.fd = sfdNew;
                epoll_ctl(epfd, EPOLL_CTL_ADD, sfdNew, &ev);
                
                pNode = (pNode_t)calloc(1, sizeof(Node_t));
                pNode->_sfdNew = sfdNew;
                strcpy(pNode->_ip, inet_ntoa(cli.sin_addr));
                pNode->_port = ntohs(cli.sin_port);
                strcpy(pNode->_path, ROOTPATH);
                pNode->_idxLen = strlen(pNode->_path);
                pNode->_inum = 0;
                pNode->_lenDir[pNode->_inum] = strlen(pNode->_path);
                
                pthread_mutex_lock(&factory._que._mutex);
                taskQueInsertTail(&factory._que, pNode);
                pthread_mutex_unlock(&factory._que._mutex);
                
                //pthread_cond_signal(&factory._cond);
            }

            while (pTemp)
            {
                if (pTemp->_sfdNew == evs[idx].data.fd) {
                    pCur = pTemp;

                    if (0 == pCur->_flagSigninStatus) { // Sign in or Sign up
                        ret = recvN(pCur->_sfdNew, &flag, sizeof(char));
                        if (-1 == ret || 0 == ret) {
                            continue;
                        }

                        if (-1 == flag) { // Disconnected
                            bzero(&ev, sizeof(struct epoll_event));
                            ev.events = EPOLLIN;
                            ev.data.fd = pCur->_sfdNew;
                            epoll_ctl(epfd, EPOLL_CTL_DEL, pCur->_sfdNew, &ev);
                            close(pCur->_sfdNew);
                            taskQueDelete(&factory._que._pHead, &factory._que._pTail, pCur);
                            printf("Disconnected\n");
                        } else if (1 == flag || 2 == flag) { // Sign in or Sign up
                            pCur->_flagSigninStatus = flag;
                        }
                    } else if (1 == pCur->_flagSigninStatus) { // Sign in
                        ret = verifySignInInfo(pCur);
                        if (-1 == ret) {
                            syslog(LOG_INFO, "[user] %s [info] %s\n", pCur->_user, "Sign out");
                            bzero(&ev, sizeof(struct epoll_event));
                            ev.events = EPOLLIN;
                            ev.data.fd = pCur->_sfdNew;
                            epoll_ctl(epfd, EPOLL_CTL_DEL, pCur->_sfdNew, &ev);
                            close(pCur->_sfdNew);
                            
                            pTemp = pTemp->_pNext;
                            
                            taskQueDelete(&factory._que._pHead, &factory._que._pTail, pCur);
                            printf("Sign out\n");
                            
                            continue;
                        }
                    } else if (2 == pCur->_flagSigninStatus) { // Sign up
                        ret = signUp(pCur);
                        if (-1 == ret) {
                            syslog(LOG_INFO, "[user] %s [info] %s\n", pCur->_user, "Sign out");
                            bzero(&ev, sizeof(struct epoll_event));
                            ev.events = EPOLLIN;
                            ev.data.fd = pCur->_sfdNew;
                            epoll_ctl(epfd, EPOLL_CTL_DEL, pCur->_sfdNew, &ev);
                            close(pCur->_sfdNew);
                            
                            pTemp = pTemp->_pNext;
                            
                            taskQueDelete(&factory._que._pHead, &factory._que._pTail, pCur);
                            printf("Sign out\n");
                            
                            continue;
                        }
                    } else if (7 == pCur->_flagSigninStatus) { // Sign in succeeded
                        ret = getCommand(pCur, &factory);
                        if (-1 == ret) {
                            syslog(LOG_INFO, "[user] %s [info] %s\n", pCur->_user, "Sign out");
                            bzero(&ev, sizeof(struct epoll_event));
                            ev.events = EPOLLIN;
                            ev.data.fd = pCur->_sfdNew;
                            epoll_ctl(epfd, EPOLL_CTL_DEL, pCur->_sfdNew, &ev);
                            close(pCur->_sfdNew);
                            
                            pTemp = pTemp->_pNext;
                            
                            taskQueDelete(&factory._que._pHead, &factory._que._pTail, pCur);
                            printf("Sign out\n");
                            
                            continue;
                        } else if (0 == ret) {
                            if ('u' == pCur->_flagCmd || 'd' == pCur->_flagCmd) {
                                bzero(&ev, sizeof(struct epoll_event));
                                ev.events = EPOLLIN;
                                ev.data.fd = pCur->_sfdNew;
                                epoll_ctl(epfd, EPOLL_CTL_DEL, pCur->_sfdNew, &ev);
                                
                                sfdNew = accept(sfd, (struct sockaddr*)&cli, &addrlen);
                                
                                bzero(&ev, sizeof(struct epoll_event));
                                ev.events = EPOLLIN;
                                ev.data.fd = sfdNew;
                                epoll_ctl(epfd, EPOLL_CTL_ADD, sfdNew, &ev);

                                pCur->_flagCmd = 0;
                                taskQueModify(factory._que._pHead, pCur->_sfdNew, sfdNew);
                            }
                        }
                    }
                }
                pTemp = pTemp->_pNext;
            }

            if (exitfd[0] == evs[idx].data.fd) { // 退出机制可封装
                bzero(&ev, sizeof(struct epoll_event));
                ev.events = EPOLLIN;
                ev.data.fd = sfd;
                epoll_ctl(epfd, EPOLL_CTL_DEL, sfd, &ev);
                close(sfd);
            
                for (int idx = 0; idx != numThread; ++idx)
                {
                    pthread_cancel(factory._pThreadId[idx]);
                    printf("cancel pthid = %lu\n", factory._pThreadId[idx]);
                    pthread_join(factory._pThreadId[idx], NULL);
                    printf("join pthid = %lu\n", factory._pThreadId[idx]);
                }
                //while (1);
                taskQueDestory(&factory._que);
                goto LabelExit;
            }
        }
    }
LabelExit:
    for (idx = 0; idx != 3; ++idx)
    {
        free(argvConf[idx]);
    }
    free(argvConf);
    
    closelog();

    return 0;
}

int main(int argc, char* argv[])
{
    return (AppInit(argc, argv) ? -1 : 0);
}
