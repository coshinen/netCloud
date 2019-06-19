 ///
 /// @file    command.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-16 15:44:58
 ///

#include "head.h"

void getCommand(pNode_t pNode, char ** argv)
{
    int epfd = epoll_create(1);
    struct epoll_event ev, evs[2];
    bzero(&ev, sizeof(struct epoll_event));
    ev.events = EPOLLIN;
    ev.data.fd = pNode->_sfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, pNode->_sfd, &ev);
    bzero(&ev, sizeof(struct epoll_event));
    ev.events = EPOLLIN;
    ev.data.fd = 0;
    epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &ev);

    int retEpoll, idx;
    Train_t train;
    char ** cmd = NULL;
    ssize_t ret;
    char flag = 0;
    idxBash = strlen(bash);
    while (1)
    {
        if (0 == flag) {
            printf("[%s]$>", bash);
            fflush(stdout);
        }
        bzero(evs, sizeof(evs));
        retEpoll = epoll_wait(epfd, evs, 2, -1);
        
        for (idx = 0; idx != retEpoll; ++idx)
        {
            if (0 == evs[idx].data.fd) {
                --flag;

                bzero(&train, sizeof(Train_t));
                read(0, train._buf, sizeof(train._buf));
                train._len = strlen(train._buf) - 1;
                train._buf[train._len] = 0;
                cmd = parseCommand(train._buf);
                if (-1 == verifyCommand(cmd)) {
                    ++flag;
                }
                
                sendN(pNode->_sfd, (char*)&train, sizeof(train._len) + train._len);
                if (0 == train._len) {
                    break;
                }
            }

            if (pNode->_sfd == evs[idx].data.fd) {
                ++flag;

                ret = selectCommand(cmd, pNode);
                if (cmd != NULL) {
                    for (size_t idx = 0; idx != 3; ++idx)
                    {
                        free(cmd[idx]);
                    }
                    free(cmd);
                    cmd = NULL;
                }
                if ('d' == pNode->_flagCmd || 'u' == pNode->_flagCmd) {
                    bzero(&ev, sizeof(struct epoll_event));
                    ev.events = EPOLLIN;
                    ev.data.fd = pNode->_sfd;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, pNode->_sfd, &ev);

                    sleep(1);
                    pNode->_sfd = scSocket(argv);
                    pNode->_flagCmd = 0;
                    
                    bzero(&ev, sizeof(struct epoll_event));
                    ev.events = EPOLLIN;
                    ev.data.fd = pNode->_sfd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, pNode->_sfd, &ev);
                }
                printf("ret = %ld\n", ret);
            }
        }
        
        if (-1 == ret) {
            printf("Sign out\n");
            break;
        }
    }
}

char ** parseCommand(const char * buf)
{
    if (!strcmp("", buf)) {
        return NULL;
    }

    char ** cmd = (char**)calloc(3, sizeof(char*));
    size_t idx;
    for (idx = 0; idx != 3; ++idx)
    {
        cmd[idx] = (char*)calloc(1, sizeof(char) * 64);
    }
    
    size_t iBuf;
    for (iBuf = 0; iBuf != strlen(buf); ++iBuf)
    {
        if (buf[iBuf] != ' ') {
                break;
        }
    }

    idx = 0;
    for (size_t jCmd = 0; iBuf != strlen(buf); ++iBuf)
    {
        if (buf[iBuf] != ' ') {
            cmd[idx][jCmd] = buf[iBuf];
            ++jCmd;
            if (buf[iBuf] != ' ' && buf[iBuf + 1] == ' ') {
                ++idx;
                if (3 == idx) {
                    break;
                }
            }
        } else {
            jCmd = 0;
        }
    }
    
    return cmd;
}

ssize_t verifyCommand(char ** cmd)
{
    if (NULL == cmd) {
        return -1;
    }

    if (!strcmp("help", *cmd)) {
    } else if (!strcmp("ls", *cmd)) {
    } else if (!strcmp("pwd", *cmd)) {
    } else if (!strcmp("cd", *cmd)) {
    } else if (!strcmp("gets", *cmd)) {
    } else if (!strcmp("puts", *cmd)) {
    } else if (!strcmp("remove", *cmd)) {
    } else if (!strcmp("mkdir", *cmd)) {
    } else if (!strcmp("rmdir", *cmd)) {
    } else if (!strcmp("rename", *cmd)) {
    } else if (!strcmp("exit", *cmd) || !strcmp("signout", *cmd)) {
    } else {
        printf("No command '%s' found\n", *cmd);
        return -1;
    }
    return 0;
}

ssize_t selectCommand(char ** cmd, pNode_t pNode)
{
    if (NULL == cmd) {
        return -1;
    }

    ssize_t flag = 0;
    pthread_t pthId;
    pthread_attr_t attr;
    
    if (!strcmp("help", *cmd)) {
        helpFile(pNode->_sfd);
    } else if (!strcmp("ls", *cmd)) {
        listFiles(pNode->_sfd, cmd);
    } else if (!strcmp("pwd", *cmd)) {
        printWorkingDirectory(pNode->_sfd);
    } else if (!strcmp("cd", *cmd)) {
        changeDirectory(pNode->_sfd, cmd[1]);
    } else if (!strcmp("gets", *cmd)) {
        pNode->_flagCmd = 'd';
        bzero(pNode->_fileName, sizeof(pNode->_fileName));
        strcpy(pNode->_fileName, cmd[1]);
        pNode->_sfdTmp = pNode->_sfd;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&pthId, &attr, threadHandler, pNode);
        //return getsFile(pNode->_sfd, cmd[1]);
    } else if (!strcmp("puts", *cmd)) {
        pNode->_flagCmd = 'u';
        bzero(pNode->_fileName, sizeof(pNode->_fileName));
        strcpy(pNode->_fileName, cmd[1]);
        pNode->_sfdTmp = pNode->_sfd;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&pthId, &attr, threadHandler, pNode);
        //return putsFile(pNode->_sfd, cmd[1]);
    } else if (!strcmp("remove", *cmd)) {
        removeFile(pNode->_sfd, cmd[1]);
    } else if (!strcmp("mkdir", *cmd)) {
        makeDirectory(pNode->_sfd, cmd[1]);
    } else if (!strcmp("rmdir", *cmd)) {
        removeDirectory(pNode->_sfd, cmd[1]);
    } else if (!strcmp("rename", *cmd)) {
        renameFile(pNode->_sfd, *cmd);
    } else if (!strcmp("exit", *cmd) || !strcmp("signout", *cmd)) {
        flag = signOut(pNode->_sfd);
    }
    
    return flag;
}

ssize_t sendN(int sfd, const char * train, size_t len)
{
    ssize_t total = 0;
    ssize_t ret;
    while ((size_t)total < len)
    {
        ret = send(sfd, train + total, len - (size_t)total, 0);
        if (-1 == ret) {
            return -1;
        }
        total += ret;
    }
    return total;
}

ssize_t recvN(int sfd, char * train, size_t len)
{
    ssize_t total = 0;
    ssize_t ret;
    while ((size_t)total < len)
    {
        ret = recv(sfd, train + total, len - (size_t)total, 0);
        if (-1 == ret) {
            perror("recv");
            return -1;
        } else if (0 == ret) {
            return 0;
        }
        total += ret;
    }
    return total;
}

ssize_t getMD5(int fd, off_t sizeFile, char * md5)
{
    unsigned char * pMmap = (unsigned char*)mmap(NULL, sizeFile, PROT_READ, MAP_SHARED, fd, 0);
    if (MAP_FAILED == (void*)pMmap) {
        perror("getsMmap");
        return -1;
    }

    unsigned char md[16];
    MD5(pMmap, sizeFile, md);
    char temp[3] = {0};
    for (size_t idx = 0; idx != 16; ++idx)
    {
        sprintf(temp, "%2.2x", md[idx]);
        strcat(md5, temp);
    }
    
    if (-1 == munmap(pMmap, sizeFile)) {
        perror("getsMunmap");
        return -1;
    }

    return 0;
}

size_t int2str(char * buf, off_t size)
{
    size_t len = 0;
    off_t tmpSize = size;
    while (tmpSize != 0)
    {
        tmpSize /= 10;
        ++len;
    }
    
    off_t tmp;
    for (size_t idx = len - 1; size != 0; --idx)
    {
        tmp = size % 10;
        size /= 10;
        buf[idx] = '0' + tmp;
    }
    
    return len;
}

size_t float2str(char * buf, off_t size)
{
    size_t len = 0;
    off_t tmpSize = size;
    while (tmpSize != 0)
    {
        tmpSize /= 10;
        ++len;
    }
    
    off_t tmp;
    for (size_t idx = len - 1; size != 0; --idx)
    {
        tmp = size % 10;
        size /= 10;
        buf[idx] = '0' + tmp;
    }
    
    buf[len] = buf[len - 1];
    buf[len - 1] = buf[len - 2];
    buf[len - 2] = '.';
    
    return len + 1;
}

void convertSize(char * buf, double size)
{
    if (size < 1 << 10) { // B
        buf[float2str(buf, size * 100)] = 'B';
    } else if (size < 1 << 20) { // K
        size /= 1 << 10;
        buf[float2str(buf, size * 100)] = 'K';
    } else if (size < 1 << 30) { // M
        size /= 1 << 20;
        buf[float2str(buf, size * 100)] = 'M';
    } else { // G
        size /= 1 << 30;
        buf[float2str(buf, size * 100)] = 'G';
    }
}

size_t float2strSpeed(char * buf, off_t size)
{
    size_t len = 0;
    off_t tmpSize = size;
    while (tmpSize != 0)
    {
        tmpSize /= 10;
        ++len;
    }
    
    off_t tmp;
    for (size_t idx = len - 1; size != 0; --idx)
    {
        tmp = size % 10;
        size /= 10;
        buf[idx] = '0' + tmp;
    }
    
    buf[len] = buf[len - 1];
    buf[len - 1] = '.';
    
    return len + 1;
}

void convertSizeSpeed(char * buf, double size)
{
    size_t idx;
    if (size < 1 << 10) { // B
        idx = float2strSpeed(buf, size * 10);
        buf[idx] = 'B';
    } else if (size < 1 << 20) { // K
        size /= 1 << 10;
        idx = float2strSpeed(buf, size * 10);
        buf[idx] = 'K';
        buf[++idx] = 'B';
    } else if (size < 1 << 30) { // M
        size /= 1 << 20;
        idx = float2strSpeed(buf, size * 10);
        buf[idx] = 'M';
        buf[++idx] = 'B';
    } else { // G
        size /= 1 << 30;
        idx = float2strSpeed(buf, size * 10);
        buf[idx] = 'G';
        buf[++idx] = 'B';
    }
    buf[idx + 1] = '/';
    buf[idx + 2] = 's';
}

void helpFile(int sfd)
{
    char flag;
    recvN(sfd, &flag, sizeof(char));

    printf("Command is as follows.\n"
            "Type `ls' to list the files under the current path.\n"
            "or `pwd' to print the current wroking directory.\n"
            "or `cd ...' to Change the current directory to the specified another(...).\n"
            "or `gets ...' to download the file(...) from the current path.\n"
            "or `puts ...' to upload the file(...) to the current path.\n"
            "or `remove ...' to delete the file(...) from the current path.\n"
            "or `mkdir ...' to create directory(...) under the current path.\n"
            "or `rmdir ...' to delete empty directory(...) from the current path.\n"
            "or `rename oldname newname' to rename file or directory from oldname to newname.\n"
            "or `exit' to sign out the SkyDrive.\n");
}

void print(int sfd)
{
    Train_t train;
    while (1)
    {
        bzero(&train, sizeof(Train_t));
        recvN(sfd, (char*)&train._len, sizeof(size_t));
        if (train._len != 0) {
            recvN(sfd, train._buf, train._len);
            printf("%s\n", train._buf);
        } else {
            break;
        }
    }
}

void listFiles(int sfd, char ** cmd)
{
    print(sfd);

    char flag = 0;
    recvN(sfd, &flag, sizeof(char));
    if (-1 == flag) {
        printf("ls: cannot access '%s': No such file or directory\n", cmd[1]);
    } else if (-2 == flag) {
        printf("ls: cannot access '%s': No such file or directory\n", cmd[2]);
    }
}

void printWorkingDirectory(int sfd)
{
    print(sfd);
}

void changeDirectory(int sfd, const char * directory)
{
    char flag = -1;
    recvN(sfd, &flag, sizeof(char));
    if (-1 == flag) {
        printf("-bash: cd: %s: No such file or directory\n", directory);
    } else {
        Train_t train;
        while (1)
        {
            bzero(&train, sizeof(Train_t));
            recvN(sfd, (char*)&train._len, sizeof(size_t));
            if (train._len != 0) {
                recvN(sfd, train._buf, train._len);
                printf("%s\n", train._buf);
                sprintf(bash + idxBash, "%s", train._buf + 1);
                bash[idxBash + train._len - 1] = 0;
            } else {
                break;
            }
        }
    }
}

void writeTempConf(int fdTemp, const char * downloadPath, const char * fileSize, const char * total)
{
    lseek(fdTemp, 0, SEEK_SET);
    char temp[1024] = {0};
    sprintf(temp,
            "{\n"
            "    \"DownloadPath\": \"%s\",\n"
            "    \"FileSize\": \"%s\",\n"
            "    \"From\": \"%s\",\n"
            "    \"To\": \"%s\"\n"
            "}",
            downloadPath, fileSize, total, fileSize);
    write(fdTemp, temp, strlen(temp));
}

void sighandler(int signum)
{
    printf("signum = %d\n", signum);
    FLAG = -1;
}

ssize_t getsFileAgain(int sfd, int fd, const char * fileName, off_t sizeFile, int fdTemp, const char * downloadPath, const char * strSizeFile, char * strTotalCur, const char * fileNameDownloading)
{
    char ** args = readDownloadingConf(fileNameDownloading);

    off_t sizeFileCur = atol(args[2]);
    sendN(sfd, (char*)&sizeFileCur, sizeof(off_t));

    ssize_t ret;
    if (sizeFile > 100 * 1024 * 1024) { // mmap
        ret = getsMappingLargeFile(sfd, sizeFile, fd, fileName, sizeFileCur, fdTemp, downloadPath, strSizeFile, strTotalCur);
        if (-1 == ret) {
            return -1;
        }
        return 0;
    } else {
        lseek(fd, sizeFileCur, SEEK_SET);
        off_t totalCur = sizeFileCur;
        ssize_t retLen = 0;
        char buf[1020];
        while (totalCur < sizeFile)
        {
            bzero(buf, sizeof(buf));
            ret = recv(sfd, buf, sizeof(buf), 0);
            if (-1 == ret || 0 == ret) {
                return -1;
            }
            retLen = write(fd, buf, ret);
            totalCur += retLen;
            printf("\rDownloading...%5.2f%%", ((double)totalCur / sizeFile) * 100);
            fflush(stdout);
            int2str(strTotalCur, totalCur);
            writeTempConf(fdTemp, downloadPath, strSizeFile, strTotalCur);
            if (-1 == FLAG) {
                return -1;
            }
        }
    }

    return 0;
}

ssize_t getsMappingLargeFile(int sfd, off_t sizeFile, int fd, const char * fileName, off_t sizeFileCur, int fdTemp, const char * downloadPath, const char * strSizeFile, char * strTotalCur)
{
    if (0 == sizeFileCur) {
        int ret = ftruncate(fd, sizeFile);
        if (-1 == ret) {
            perror("ftruncate");
            return 0;
        }
    }

    char * pMmap = (char*)mmap(NULL, sizeFile, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == (void*)pMmap) {
        perror("mmap");
        return 0;
    }

    off_t totalCur = sizeFileCur;
    ssize_t retLen = 0;
    char sizeFileStr[9] = {0};
    convertSize(sizeFileStr, (double)sizeFile);
    char totalCurStr[9];
    char speed[11];
    time_t begin;
    time(&begin);
    time_t cur;
    while (totalCur < sizeFile)
    {
        retLen = recv(sfd, pMmap + totalCur, sizeFile - totalCur, 0);
        if (-1 == retLen || 0 == retLen) {
            if (-1 == munmap(pMmap, sizeFile)) {
                perror("munmap");
                return 0;
            }
            return -1;
        }
        totalCur += retLen;
        
        bzero(totalCurStr, sizeof(totalCurStr));
        convertSize(totalCurStr, (double)totalCur);
        time(&cur);
        if (cur - begin >= 1) {
            begin = cur;

            bzero(speed, sizeof(speed));
            convertSizeSpeed(speed, (double)retLen);
            printf("\rdownloading: [%s %s/%s %5.2f%%] %s   ", fileName, totalCurStr, sizeFileStr, ((double)totalCur / sizeFile) * 100, speed);
            fflush(stdout);
        }
        
        int2str(strTotalCur, totalCur);
        writeTempConf(fdTemp, downloadPath, strSizeFile, strTotalCur);
        if (-1 == FLAG || 0 == retLen) {
            if (-1 == munmap(pMmap, sizeFile)) {
                perror("munmap");
                return 0;
            }
            return -1;
        }
    }
    printf("\rdownload completed: [%s %s/%s %5.2f%%] %s\n", fileName, totalCurStr, sizeFileStr, ((double)totalCur / sizeFile) * 100, "0K/s");

    if (-1 == munmap(pMmap, sizeFile)) {
        perror("munmap");
        return 0;
    }

    return 0;
}

ssize_t getsFile(int sfd, const char * fileName)
{
    signal(SIGINT, sighandler);

    char flag = 0;
    recvN(sfd, &flag, sizeof(char));
    if (-1 == flag) {
        printf("gets: failed to gets '%s': No such file or directory\n", fileName);
    } else if (4 ==flag) {
        printf("gets: failed to gets '%s': Is a directory\n", fileName);
    } else if (8 == flag) {
        size_t sizeFile;
        recvN(sfd, (char*)&sizeFile, sizeof(size_t));
    
        char * path = getcwd(NULL, 0); // open preparation
        char downloadPath[1024] = {0};
        sprintf(downloadPath, "%s%s%s", path, "/", fileName);
        char fileNameDownloading[1024] = {0};
        sprintf(fileNameDownloading, "%s%s", fileName, ".downloading");
    
        char inum = 0;
        DIR * dir = opendir("."); // getsFileAgain preparation
        struct dirent * pDir;
        while ((pDir = readdir(dir)) != NULL)
        {
            if (!strcmp(pDir->d_name, fileName) || !strcmp(pDir->d_name, fileNameDownloading)) {
                ++inum;
            }
        } // inum = 2,getsFileAgain
        sendN(sfd, &inum, sizeof(char));

        int fd = open(downloadPath, O_CREAT | O_RDWR, 0666);
        int fdTemp = open(fileNameDownloading, O_CREAT | O_RDWR, 0666);
        
        char strSizeFile[16] = {0}; // writeTemp preparation
        int2str(strSizeFile, sizeFile);
        char strTotalCur[16] = {0};

        ssize_t ret;
        if (2 == inum) { // select download ways
            ret = getsFileAgain(sfd, fd, fileName, sizeFile, fdTemp, downloadPath, strSizeFile, strTotalCur, fileNameDownloading);
            closedir(dir);
            close(fd);
            close(fdTemp);
            if (-1 == ret) {
                return -1;
            } else if (0 == ret) {
                unlink(fileNameDownloading);
                printf("\nDownload succeeded\n");
                return 0;
            }
        }
        
        if (sizeFile > 100 * 1024 * 1024) { // mmap
            ret = getsMappingLargeFile(sfd, sizeFile, fd, fileName, 0, fdTemp, downloadPath, strSizeFile, strTotalCur);
            closedir(dir);
            close(fd);
            close(fdTemp);
            if (-1 == ret) {
                return -1;
            } else if (0 == ret) {
                unlink(fileNameDownloading);
                return 0;
            }
        } else { // download
            off_t totalCur = 0;
            ssize_t retLen = 0;
            char buf[1020];
            char sizeFileStr[9] = {0};
            convertSize(sizeFileStr, (double)sizeFile);
            char totalCurStr[9];
            char speed[11];
            time_t begin;
            time(&begin);
            time_t cur;
            while ((size_t)totalCur < sizeFile)
            {
                bzero(buf, sizeof(buf));
                ret = recv(sfd, buf, sizeof(buf), 0);
                if (-1 == ret || 0 == ret) {
                    closedir(dir);
                    close(fd);
                    close(fdTemp);
                    return -1;
                }
                retLen = write(fd, buf, ret);
                totalCur += retLen;
            
                bzero(totalCurStr, sizeof(totalCurStr));
                convertSize(totalCurStr, (double)totalCur);
                time(&cur);
                if (cur - begin >= 1) {
                    begin = cur;

                    bzero(speed, sizeof(speed));
                    convertSizeSpeed(speed, (double)retLen);
                    printf("\rdownloading: [%s %s/%s %5.2f%%] %s   ", fileName, totalCurStr, sizeFileStr, ((double)totalCur / sizeFile) * 100, speed);
                    fflush(stdout);
                }
                
                int2str(strTotalCur, totalCur);
                writeTempConf(fdTemp, downloadPath, strSizeFile, strTotalCur);
                if (-1 == FLAG) {
                    return -1;
                }
            }
            closedir(dir);
            close(fd);
            close(fdTemp);
            unlink(fileNameDownloading);
            printf("\rdownload completed: [%s %s/%s %5.2f%%] %s\n", fileName, totalCurStr, sizeFileStr, ((double)totalCur / sizeFile) * 100, "0K/s");
        }
    }

    return 0;
}

ssize_t putsMappingLargeFile(int sfd, off_t sizeFile, int fd)
{
    char * pMmap = (char*)mmap(NULL, sizeFile, PROT_READ, MAP_SHARED, fd, 0);
    if (MAP_FAILED == (void*)pMmap) {
        perror("mmap");
        return -1;
    }
    
    off_t totalCur = 0;
    ssize_t retLen = 0;
    while (totalCur < sizeFile)
    {
        retLen = send(sfd, pMmap + totalCur, sizeFile - totalCur, 0);
        if (-1 == retLen) {
            if (-1 == munmap(pMmap, sizeFile)) {
                perror("munmap");
                return -1;
            }
            return -1;
        }
        totalCur += retLen;
        printf("\rDownloading...%5.2f%%", ((double)totalCur / sizeFile) * 100);
        fflush(stdout);
    }
    
    if (-1 == munmap(pMmap, sizeFile)) {
        perror("munmap");
        return -1;
    }

    return 0;
}

ssize_t putsFile(int sfd, const char * fileName)
{
    char flag = -1;
    recvN(sfd, &flag, sizeof(char));

    int fd = open(fileName, O_RDONLY);
    if (-1 == fd) {
        perror("open");
        flag = -1;
        sendN(sfd, &flag, sizeof(char));
        return 0;
    }
    sendN(sfd, &flag, sizeof(char));

    struct stat st;
    bzero(&st, sizeof(struct stat));
    fstat(fd, &st);
    
    char md5[33] = {0};
    getMD5(fd, st.st_size, md5);
    sendN(sfd, md5, sizeof(md5));

    recvN(sfd, &flag, sizeof(char));
    if (1 == flag) { // not found, continue uploading
        printf("Upload started\n");
    } else if (-2 == flag) {
        close(fd);
        printf("puts: failed to puts '%s': File exists\n", fileName);
        return 0;
    } else if (-1 == flag) {
        close(fd);
        printf("Upload failed\n");
        return 0;
    } else if (0 == flag) {
        printf("Upload started\n");
        printf("Downloading...100%%\n");
        close(fd);
        printf("Upload succeeded\n");
        return 0;
    }

    ssize_t ret = sendN(sfd, (char*)&st.st_size, sizeof(off_t));
    if (-1 == ret) {
        close(fd);
        return -1;
    }

    if (st.st_size > 100 * 1024 * 1024) { // mmap
        ret = putsMappingLargeFile(sfd, st.st_size, fd);
        if (-1 == ret) {
            close(fd);
            printf("\nUpload failed\n");
            return -1;
        }
    } else {
        off_t totalCur = 0; // upload
        ssize_t retLen = 0;
        while (totalCur < st.st_size)
        {
            retLen = sendfile(sfd, fd, &totalCur, (size_t)st.st_size - (size_t)totalCur);
            if (-1 == retLen) {
                close(fd);
                return -1;
            }
            printf("\rDownloading...%5.2f%%", ((double)totalCur / st.st_size) * 100);
            fflush(stdout);
        }
    }

    recvN(sfd, &flag, sizeof(char));
    if (-1 == flag) {
        printf("\nUpload failed\n");
    } else if (0 == flag) {
        printf("\nUpload succeeded\n");
    }
    close(fd);
    return 0;
}

void removeFile(int sfd, char * fileName)
{
    char flag;
    recvN(sfd, &flag, sizeof(char));
    
    if (-1 == flag) {
        printf("remove: cannot remove '%s': No such file or directory\n", fileName);
    } else if (-2 == flag) {
        printf("remove: cannot remove '%s': Is a directory\n", fileName);
    } else if (0 == flag) {
        printf("remove: remove '%s': succeeded\n", fileName);
    }
}

void makeDirectory(int sfd, char * directory)
{
    char flag;
    recvN(sfd, &flag, sizeof(char));

    if (!strcmp("", directory)) {
        printf("mkdir: missing operand\n"
                "Try `help' for more information.\n");
    } else {
        if (-1 == flag) {
            printf("mkdir: cannot create directory '%s': Failed\n", directory);
        } else if (0 == flag) {
            printf("mkdir: make directory '%s': Succeeded\n", directory);
        } else if (1 == flag) {
            printf("mkdir: cannot create directory '%s': File exists\n", directory);
        }
    }
}

void removeDirectory(int sfd, char * directory)
{
    char flag;
    recvN(sfd, &flag, sizeof(char));

    if (!strcmp("", directory)) {
        printf("rmdir: missing operand\n"
                "Try `help' for more information.\n");
    } else {
        if (-1 == flag) {
            printf("rmdir: failed to remove '%s': Failed\n", directory);
        } else if (0 == flag) {
            printf("rmdir: remove directory '%s': Succeeded\n", directory);
        } else if (1 == flag) {
            printf("rmdir: failed to remove '%s': No such file or directory\n", directory);
        } else if (2 == flag) {
            printf("rmdir: failed to remove '%s': Directory not empty\n", directory);
        } else if (8 == flag) {
            printf("rmdir: failed to remove '%s': Not a directory\n", directory);
        }
    }
}

void renameFile(int sfd, char * fileName)
{
    char flag;
    recvN(sfd, &flag, sizeof(char));
    
    if (-1 == flag) {
        printf("rename: failed to rename '%s': Failed\n", fileName);
    } else if (0 == flag) {
        printf("rename: rename '%s': Succeeded\n", fileName);
    } else if (1 == flag) {
        printf("rename: failed to rename '%s': No such file or directory\n", fileName);
    }
}

ssize_t signOut(int sfd)
{
    ssize_t flag = 0;
    recvN(sfd, (char*)&flag, sizeof(ssize_t));
    return flag;
}
