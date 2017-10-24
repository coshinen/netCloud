 ///
 /// @file    command.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-13 21:01:03
 ///

#include "../include/threadHandler.h"

ssize_t getCommand(pNode_t pNode, pFactory_t pFactory)
{
	Train_t train;
	char ** cmd;
	ssize_t ret;
	bzero(&train, sizeof(Train_t));

	ret = recvN(pNode->_sfdNew, (char*)&train._len, sizeof(size_t));
	if (0 == ret) {
		return -1;
	} else if (0 == train._len) {
		return 0;
	}
	ret = recvN(pNode->_sfdNew, train._buf, train._len);
	if (0 == ret) {
		return -1;
	}
	
	cmd = parseCommand(train._buf);

	syslog(LOG_INFO, "[user] %s [command] %s %s %s\n", pNode->_user, cmd[0], cmd[1], cmd[2]);

	ret = selectCommand(cmd, pNode, pFactory);
	
	for (size_t idx = 0; idx != 3; ++idx)
	{
		free(cmd[idx]);
	}
	free(cmd);
	
	if (-1 == ret) {
		return -1;
	}

	return 0;
}

char ** parseCommand(const char * buf)
{
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

void getDate(char * date)
{
	time_t t;
	time(&t);
	struct tm * pgm = gmtime(&t);
	sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", 1900 + pgm->tm_year, 1 + pgm->tm_mon, pgm->tm_mday, 8 + pgm->tm_hour, pgm->tm_min, pgm->tm_sec);
}

ssize_t selectCommand(char ** cmd, pNode_t pNode, pFactory_t pFactory)
{
	ssize_t flag = 0;
	pNode_t pCur = (pNode_t)calloc(1, sizeof(Node_t));
	
	if (!strcmp("help", *cmd)) {
		helpFile(pNode);
	} else if (!strcmp("ls", *cmd)) {
		listFiles(pNode, cmd);
	} else if (!strcmp("pwd", *cmd)) {
		printWorkingDirectory(pNode);
	} else if (!strcmp("cd", *cmd)) {
		changeDirectory(pNode, cmd[1]);
	} else if (!strcmp("gets", *cmd)) {
		bzero(pNode->_fileName, sizeof(pNode->_fileName));
		strcpy(pNode->_fileName, cmd[1]);
		pNode->_flagCmd = 'd';
		memcpy(pCur, pNode, sizeof(Node_t));
		pthread_mutex_lock(&pFactory->_queFile._mutex);
		taskQueInsertTail(&pFactory->_queFile, pCur);
		pthread_mutex_unlock(&pFactory->_queFile._mutex);
		pthread_cond_signal(&pFactory->_cond);
		//return getsFile(pNode, cmd[1]);
	} else if (!strcmp("puts", *cmd)) {
		bzero(pNode->_fileName, sizeof(pNode->_fileName));
		strcpy(pNode->_fileName, cmd[1]);
		pNode->_flagCmd = 'u';
		memcpy(pCur, pNode, sizeof(Node_t));
		pthread_mutex_lock(&pFactory->_queFile._mutex);
		taskQueInsertTail(&pFactory->_queFile, pCur);
		pthread_mutex_unlock(&pFactory->_queFile._mutex);
		pthread_cond_signal(&pFactory->_cond);
		//return putsFile(pNode, cmd[1]);
	} else if (!strcmp("remove", *cmd)) {
		removeFile(pNode, cmd[1]);
	} else if (!strcmp("mkdir", *cmd)) {
		makeDirectory(pNode, cmd[1]);
	} else if (!strcmp("rmdir", *cmd)) {
		removeDirectory(pNode, cmd[1]);
	} else if (!strcmp("rename", *cmd)) {
		renameFile(pNode, cmd[1], cmd[2]);
	} else if (!strcmp("exit", *cmd) || !strcmp("signout", *cmd)) {
		flag = signOut(pNode);
	} else {
		printf("No this command!\n");
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

void helpFile(pNode_t pNode)
{
	char flag = 0;
	sendN(pNode->_sfdNew, &flag, sizeof(char));
}

ssize_t listFiles(pNode_t pNode, char ** cmd)
{
	char path[256] = {0};
	strncpy(path, pNode->_path, pNode->_idxLen - 1);
	size_t curInode;
	verifyMysqlFileSystem(&curInode, NULL, path, NULL, NULL, NULL);
	size_t preInode = curInode;
	char type[5] = {0};
	char fileName[64] = {0};
	size_t sizeFile;
	char date[20] = {0};
	Train_t train;
	char sizeStr[9];
	char flag = 0;
	
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
				if ((size_t)atol(row[1]) == preInode) {
					if (!strcmp("", cmd[1])) {
						flag = 1;

						strcpy(type, row[3]);
						strcpy(fileName, row[4]);
						sizeFile = (size_t)atol(row[9]);
						strcpy(date, row[10]);

						bzero(&train, sizeof(Train_t));
						if (!strcmp("dir", type)) {
							sprintf(train._buf, " [Dir] %-16.16s [Size] %-8s [Modified date] %s", fileName, "-", date);
						} else if (!strcmp("file", type)) {
							bzero(sizeStr, sizeof(sizeStr));
							convertSize(sizeStr, (double)sizeFile);
							sprintf(train._buf, "[File] %-16.16s [Size] %-8s [Modified date] %s", fileName, sizeStr, date);
						}
						train._len = strlen(train._buf);
						sendN(pNode->_sfdNew, (char*)&train, sizeof(size_t) + train._len);
					} else if (!strcmp("-l", cmd[1])) {
						if (!strcmp(row[4], cmd[2])) {
							flag = 1;

							strcpy(type, row[3]);
							strcpy(fileName, row[4]);
							sizeFile = (size_t)atol(row[9]);
							strcpy(date, row[10]);

							bzero(&train, sizeof(Train_t));
							if (!strcmp("dir", type)) {
								sprintf(train._buf, " [Dir] %s [Size] %-8s [Modified date] %s", fileName, "-", date);
							} else if (!strcmp("file", type)) {
								bzero(sizeStr, sizeof(sizeStr));
								convertSize(sizeStr, (double)sizeFile);
								sprintf(train._buf, "[File] %s [Size] %-8s [Modified date] %s", fileName, sizeStr, date);
							}
							train._len = strlen(train._buf);
							sendN(pNode->_sfdNew, (char*)&train, sizeof(size_t) + train._len);

							break;
						} else {
							flag = -2;
						}
					} else {
						flag = -1;
					}
				}
			}
			train._len = 0;
			sendN(pNode->_sfdNew, (char*)&train, sizeof(size_t) + train._len);
		}
		mysql_free_result(res);
	}
	mysql_close(&mysql);

	sendN(pNode->_sfdNew, &flag, sizeof(char));
	return 0;
}

void printWorkingDirectory(pNode_t pNode)
{
	Train_t train;
	bzero(&train, sizeof(Train_t));
	strcpy(train._buf, pNode->_path + strlen(ROOTPATH) + strlen(pNode->_user));
	
	if (!strcmp("/", train._buf)) {
	} else {
		train._buf[strlen(train._buf) - 1] = 0;
	}
	
	train._len = strlen(train._buf);
	sendN(pNode->_sfdNew, (char*)&train, sizeof(size_t) + train._len);

	train._len = 0;
	sendN(pNode->_sfdNew, (char*)&train, sizeof(size_t) + train._len);
}

size_t queryInum(const char * directory)
{
	size_t inum = 0;
	for (size_t idx = 0; idx != strlen(directory); ++idx)
	{
		if ('/' == directory[idx]) {
			++inum;
		}
	}
	return inum;
}

size_t getLenDir(char * tempDirectory)
{
	size_t idx;
	for (idx = strlen(tempDirectory) - 1; idx != 0; --idx)
	{
		if ('/' == tempDirectory[idx]) {
			tempDirectory[idx] = 0;
			return idx;
		}
	}
	return 0;
}

void changeDirectory(pNode_t pNode, char * directory)
{
	if (strlen(directory) != 1) {
		if ('/' == directory[strlen(directory) - 1]) { // xx / xx/
			directory[strlen(directory) - 1] = 0;
		}
	}

	char flag = -1;
	if (!strncmp("..", directory, 2)) { // verify change directory parameter
		if (!strcmp("..", directory)) {
			if (0 == pNode->_inum) { // It's a root directory
				flag = -2;
			} else {
				flag = 2; // ..
			}
		} else {
			flag = -1;
		}
	} else {
		if (!strncmp(".", directory, 1)) {
			flag = -1;
		} else {
			if (!strcmp("", directory)) {
				flag = 0;
			} else {
				if (!strncmp("/", directory, 1)) {
					char pathName[256] = {0};
					if (!strcmp("/", directory)) {
						sprintf(pathName, "%s%s", ROOTPATH, pNode->_user);
					} else {
						sprintf(pathName, "%s%s%s%s", ROOTPATH, pNode->_user, "/", directory + 1);
					}
					char type[5] = {0};
					ssize_t ret = verifyMysqlFileSystem(NULL, type, pathName, NULL, NULL, NULL);
					if (-1 == ret) {
						flag = -1;
					} else if (0 == ret) {
						if (!strcmp("dir", type)) {
							flag = 3;
						} else if (!strcmp("file", type)) {
							flag = -1;
						}
					}
				} else {
					char pathName[256] = {0};
					sprintf(pathName, "%s%s", pNode->_path, directory);
					char type[5] = {0};
					ssize_t ret = verifyMysqlFileSystem(NULL, type, pathName, NULL, NULL, NULL);
					if (-1 == ret) {
						flag = -1;
					} else if (0 == ret) {
						if (!strcmp("dir", type)) {
							flag = 1;
						} else if (!strcmp("file", type)) {
							flag = -1;
						}
					}
				}
			}
		}
	}

	sendN(pNode->_sfdNew, &flag, sizeof(char));
	
	if (-1 == flag) {
		printf("The parameter is error!\n");
		return;
	} else if (2 == flag) { // ..
		pNode->_idxLen -= pNode->_lenDir[pNode->_inum];
		pNode->_path[pNode->_idxLen] = 0;
		--pNode->_inum;
	} else if (0 == flag) { // cd
		sprintf(pNode->_path, "%s%s%s", ROOTPATH, pNode->_user, "/");
		pNode->_idxLen = strlen(ROOTPATH) + strlen(pNode->_user) + 1;
		pNode->_path[pNode->_idxLen] = 0;
		pNode->_inum = 0;
	} else if (3 == flag) { // / / /xx/xxx
		if (!strcmp("/", directory)) {
			sprintf(pNode->_path, "%s%s%s", ROOTPATH, pNode->_user, "/");
			pNode->_idxLen = strlen(ROOTPATH) + strlen(pNode->_user) + 1;
			pNode->_path[pNode->_idxLen] = 0;
			pNode->_inum = 0;
		} else {
			sprintf(pNode->_path, "%s%s%s%s%s", ROOTPATH, pNode->_user, "/", directory + 1, "/");
			pNode->_idxLen = strlen(ROOTPATH) + strlen(pNode->_user) + 1 + strlen(directory) -1 + 1;
			pNode->_path[pNode->_idxLen] = 0;
			
			char tempDirectory[256] = {0};
			sprintf(tempDirectory, "%s", directory);
			for (size_t idx = pNode->_inum + queryInum(directory) + 1; idx != pNode->_inum; --idx)
			{
				pNode->_lenDir[idx] = strlen(tempDirectory) - getLenDir(tempDirectory);
			}
			pNode->_inum = queryInum(directory);
		}
	} else if (1 == flag) { // xx / xx/xxx
		sprintf(pNode->_path + pNode->_idxLen, "%s%s", directory, "/");
		pNode->_idxLen += strlen(directory) + 1;

		char tempDirectory[256] = {0};
		sprintf(tempDirectory, "%s%s", "/", directory);
		for (size_t idx = pNode->_inum + queryInum(directory) + 1; idx != pNode->_inum; --idx)
		{
			pNode->_lenDir[idx] = strlen(tempDirectory) - getLenDir(tempDirectory);
		}
		pNode->_inum += queryInum(directory) + 1;
	}
	printWorkingDirectory(pNode);
}

ssize_t getsFileAgain(pNode_t pNode, int fd, off_t fileSize)
{
	off_t fileSizeCur;
	recvN(pNode->_sfdNew, (char*)&fileSizeCur, sizeof(off_t));
printf("fileSizeCur = %ld\n", fileSizeCur);
	
	ssize_t ret;
	if (fileSize > 100 * 1024 * 1024) { // mmap
		ret = getsMappingLargeFile(pNode, fileSize, fd, fileSizeCur);
		if (-1 == ret) {
			return -1;
		} else if (0 == ret) {
			return 0;
		}
	} else {
		off_t totalCur = fileSizeCur;
		ssize_t retLen = 0;
		while (totalCur < fileSize)
		{
			retLen = sendfile(pNode->_sfdNew, fd, &totalCur, fileSize - (size_t)totalCur);
			if (-1 == retLen) {
				close(fd);
				perror("sendfile");
				return -1;
			}
		}
	}

	return 0;
}

ssize_t getsMappingLargeFile(pNode_t pNode, off_t sizeFile, int fd, off_t sizeFileCur)
{
	char * pMmap = (char*)mmap(NULL, sizeFile, PROT_READ, MAP_SHARED, fd, 0);
	if (MAP_FAILED == (void*)pMmap) {
		perror("getsMmap");
		return 0;
	}

	off_t totalCur = sizeFileCur;
	ssize_t retLen = 0;
	while (totalCur < sizeFile)
	{
		retLen = send(pNode->_sfdNew, pMmap + totalCur, sizeFile - totalCur, 0);
		if (-1 == retLen) {
			if (-1 == munmap(pMmap, sizeFile)) {
				perror("getsMunmap");
				return 0;
			}
			return -1;
		}
		totalCur += retLen;
	}
	
	if (-1 == munmap(pMmap, sizeFile)) {
		perror("getsMunmap");
		return 0;
	}

	return 0;
}

ssize_t getsFile(pNode_t pNode, const char * fileName)
{
	char type[5] = {0};
	char pathName[256] = {0};
	sprintf(pathName, "%s%s", pNode->_path, fileName);
	char md5[33] = {0};
	size_t fileSize;
	char flag = 0;
	ssize_t ret = verifyMysqlFileSystem(NULL, type, pathName, NULL, md5, &fileSize);
	if (-1 == ret) {
		flag = -1;
		sendN(pNode->_sfdNew, &flag, sizeof(char));
		return 0;
	} else if (0 == ret) {
		if (!strcmp("dir", type)) {
			flag = 4;
			sendN(pNode->_sfdNew, &flag, sizeof(char));
			return 0;
		} else if (!strcmp("file", type)) {
			flag = 8;
			sendN(pNode->_sfdNew, &flag, sizeof(char));

			char pathNameReal[64] = {0};
			sprintf(pathNameReal, "%s%s", ROOTPATH, md5);
			int fd = open(pathNameReal, O_RDONLY);
			if (-1 == fd) {
				return 0;
			}
			ret = sendN(pNode->_sfdNew, (char*)&fileSize, sizeof(size_t));
			if (-1 == ret) {
				close(fd);
				return 0;
			}
		
			char inum = 0;
			recvN(pNode->_sfdNew, &inum, sizeof(char)); // getsFileAgain
			if (2 == inum) {
				ret = getsFileAgain(pNode, fd, fileSize);
				close(fd);
				if (-1 == ret) {
					return -1;
				} else if (0 == ret) {
					printf("Download succeeded!\n");
					return 0;
				}
			}
		
			if (fileSize > 100 * 1024 * 1024) { // mmap
				ret = getsMappingLargeFile(pNode, fileSize, fd, 0);
				close(fd);
				if (-1 == ret) {
					return -1;
				} else if (0 == ret) {
					printf("Download succeeded!\n");
					return 0;
				}
			} else { // download
				off_t totalCur = 0;
				ssize_t retLen = 0;
				while ((size_t)totalCur < fileSize)
				{
					retLen = sendfile(pNode->_sfdNew, fd, &totalCur, fileSize - (size_t)totalCur);
					//retLen = sendfile(pNode->_sfdNew, fd, NULL, fileSize - (size_t)totalCur);
					//totalCur += retLen;
					if (-1 == retLen) {
						close(fd);
						perror("sendfile");
						return -1;
					}
				}
			}
		}
	}

	return 0;
}

ssize_t putsMappingLargeFile(pNode_t pNode, off_t sizeFile, int fd)
{
	int ret = ftruncate(fd, sizeFile);
	if (-1 == ret) {
		perror("ftruncate");
		return -1;
	}

	char * pMmap = (char*)mmap(NULL, sizeFile, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (MAP_FAILED == (void*)pMmap) {
		perror("putsMmap");
		return -1;
	}

	ssize_t totalCur = 0;
	ssize_t retLen = 0;
	while (totalCur < sizeFile)
	{
		retLen = recv(pNode->_sfdNew, pMmap + totalCur, sizeFile - totalCur, 0);
		if (-1 == retLen || 0 == retLen) {
			if (-1 == munmap(pMmap, sizeFile)) {
				perror("putsMunmap");
				return -1;
			}
			return -1;
		}
		totalCur += retLen;
	}

	if (-1 == munmap(pMmap, sizeFile)) {
		perror("putsMunmap");
		return -1;
	}
	
	return 0;
}

ssize_t putsFile(pNode_t pNode, const char * fileName)
{
	char flag = 0;
	sendN(pNode->_sfdNew, &flag, sizeof(char));
	
	recvN(pNode->_sfdNew, &flag, sizeof(char));
	if (-1 == flag) {
		return 0;
	}

	char pathName[256];
	bzero(pathName, sizeof(pathName));
	sprintf(pathName, "%s%s", pNode->_path, fileName);
	
	char md5[33] = {0};
	ssize_t ret = recvN(pNode->_sfdNew, md5, sizeof(md5));
	if (0 == ret) {
		return -1;
	}
	
	size_t fileSize;
	ret = verifyMysqlFileSystem(NULL, NULL, pathName, NULL, NULL, NULL);
	if (-1 == ret) {
		size_t linkNumsFile;
		ret = verifyMysqlFileSystem(NULL, NULL, NULL, &linkNumsFile, md5, &fileSize);
		if (-1 == ret) { // not found
			flag = 1;
			sendN(pNode->_sfdNew, &flag, sizeof(char));
			printf("Upload started\n");
		} else if (0 == ret) {
			char path[256] = {0};
			strncpy(path, pNode->_path, pNode->_idxLen - 1);
			size_t curInode;
			size_t linkNumsDir;
			verifyMysqlFileSystem(&curInode, NULL, path, &linkNumsDir, NULL, NULL);
			size_t preInode = curInode;
			char date[20] = {0};
			getDate(date);
			
			ret = insertMysqlFileSystem(pNode->_user, preInode, "file", fileName, pNode->_path, pathName, linkNumsFile, md5, fileSize, date, fileSize);
			if (-1 == ret) {
				flag = -1;
				sendN(pNode->_sfdNew, &flag, sizeof(char));
				printf("Upload failed!\n");
				return 0;
			} else if (0 == ret) {
				++linkNumsDir;
				updateMysqlFileSystem(NULL, NULL, path, &linkNumsDir, NULL);
		
				++linkNumsFile;
				updateMysqlFileSystem(NULL, NULL, NULL, &linkNumsFile, md5);

				flag = 0;
				sendN(pNode->_sfdNew, &flag, sizeof(char));
				printf("Upload succeeded!\n");
				return 0;
			}
		}
	} else if (0 == ret) { // failed
		flag = -2;
		sendN(pNode->_sfdNew, &flag, sizeof(char));
		printf("The file or directory already exists!\n");
		return 0;
	}
	
	char pathNameReal[64] = {0};
	sprintf(pathNameReal, "%s%s", ROOTPATH, md5);
	int fd = open(pathNameReal, O_CREAT | O_RDWR, 0666);
	
	off_t sizeFile;
	ret = recvN(pNode->_sfdNew, (char*)&sizeFile, sizeof(off_t));
	if (0 == ret) {
		close(fd);
		return -1;
	}
	
	if (sizeFile > 100 * 1024 * 1024) { // mmap
		ret = putsMappingLargeFile(pNode, sizeFile, fd);
		if (-1 == ret) {
			close(fd);
			return -1;
		}
	} else {
		off_t totalCur = 0; // upload
		ssize_t retLen = 0;
		char buf[1020];
		while (totalCur < sizeFile)
		{
			bzero(buf, sizeof(buf));
			ret = recv(pNode->_sfdNew, buf, sizeof(buf), 0);
			if (-1 == ret || 0 == ret) {
				close(fd);
				return -1;
			}
			retLen = write(fd, buf, ret);
			totalCur += retLen;
		}
	}

	char md5Cur[33] = {0}; // verify
	getMD5(fd, sizeFile, md5Cur);
	close(fd);
	if (!strcmp(md5Cur, md5)) {
		char path[256] = {0};
		strncpy(path, pNode->_path, pNode->_idxLen - 1);
		size_t curInode;
		size_t linkNumsDir;
		verifyMysqlFileSystem(&curInode, NULL, path, &linkNumsDir, NULL, NULL);
		size_t preInode = curInode;
		char date[20] = {0};
		getDate(date);
		
		ret = insertMysqlFileSystem(pNode->_user, preInode, "file", fileName, pNode->_path, pathName, 1, md5, sizeFile, date, sizeFile);
		if (-1 == ret) {
			unlink(pathNameReal);
			flag = -1;
			sendN(pNode->_sfdNew, &flag, sizeof(char));
		} else if (0 ==ret) {
			++linkNumsDir;
			updateMysqlFileSystem(NULL, NULL, path, &linkNumsDir, NULL);
			
			flag = 0;
			sendN(pNode->_sfdNew, &flag, sizeof(char));
			printf("Upload succeeded\n");
		}
	} else {
		unlink(pathNameReal);
		flag = -1;
		sendN(pNode->_sfdNew, &flag, sizeof(char));
	}
	return 0;
}

void removeFile(pNode_t pNode, const char * fileName)
{
	char type[5] = {0};
	char pathName[256];
	bzero(pathName, sizeof(pathName));
	sprintf(pathName, "%s%s", pNode->_path, fileName);
	size_t linkNumsFile;
	char md5[33] = {0};

	char flag = -1;
	ssize_t ret = verifyMysqlFileSystem(NULL, type, pathName, &linkNumsFile, md5, NULL);
	if (-1 == ret) {
		flag = -1;
	} else if (0 == ret) {
		if (!strcmp("file", type)) {
			flag = 0;

			deleteMysqlFileSystem(pathName);
			
			char path[256] = {0};
			strncpy(path, pNode->_path, pNode->_idxLen - 1);
			size_t linkNumsDir;
			verifyMysqlFileSystem(NULL, NULL, path, &linkNumsDir, NULL, NULL);
			--linkNumsDir;
			updateMysqlFileSystem(NULL, NULL, path, &linkNumsDir, NULL);
			
			if (1 == linkNumsFile) {
				char pathNameReal[256] = {0};
				sprintf(pathNameReal, "%s%s", ROOTPATH, md5);
				unlink(pathNameReal);
			} else {
				--linkNumsFile;
				updateMysqlFileSystem(NULL, NULL, NULL, &linkNumsFile, md5);
			}
		} else if (!strcmp("dir", type)) {
			flag = -2;
		}
	}
	sendN(pNode->_sfdNew, &flag, sizeof(char));
}

void makeDirectory(pNode_t pNode, const char * fileName)
{
	char flag = -1;
	if (!strcmp("", fileName)) {
	} else {
		char path[256] = {0};
		strncpy(path, pNode->_path, pNode->_idxLen - 1);
		size_t curInode;
		size_t linkNumsDir;
		verifyMysqlFileSystem(&curInode, NULL, path, &linkNumsDir, NULL, NULL);
		size_t preInode = curInode;
		
		char pathName[256] = {0};
		sprintf(pathName, "%s%s", pNode->_path, fileName);
		
		ssize_t ret = verifyMysqlFileSystem(NULL, NULL, pathName, NULL, NULL, NULL);
		if (-1 == ret) {
			char date[20] = {0};
			getDate(date);
			
			ret = insertMysqlFileSystem(pNode->_user, preInode, "dir", fileName, pNode->_path, pathName, 0, NULL, 4096, date, 4096);
			if (-1 == ret) {
				flag = -1;
				printf("Failed to make directory!\n");
			} else if (0 == ret) {
				++linkNumsDir;
				updateMysqlFileSystem(NULL, NULL, path, &linkNumsDir, NULL);
	
				flag = 0;
				printf("Make the directory successfully!\n");
			}
		} else if (0 == ret) {
			flag = 1;
			printf("The directory or file already exists!\n");
		}
	}
	sendN(pNode->_sfdNew, &flag, sizeof(char));
}

void removeDirectory(pNode_t pNode, const char * fileName)
{
	char flag = -1;
	if (!strcmp("", fileName)) {
	} else {
		char pathName[256] = {0};
		sprintf(pathName, "%s%s", pNode->_path, fileName);
		
		char fileType[5] = {0};
		size_t linkNumsDir;
		ssize_t ret = verifyMysqlFileSystem(NULL, fileType, pathName, &linkNumsDir, NULL, NULL);
		if (-1 == ret) {
			flag = 1;
			printf("The directory doesn't exist!\n");
		} else if (0 == ret) {
			if (!strcmp("dir", fileType)) {
				if (0 == linkNumsDir) {
					ret = deleteMysqlFileSystem(pathName);
					if (-1 == ret) {
						flag = -1;
						printf("Failed to remove directory!\n");
					} else if (0 == ret) {
						char path[256] = {0};
						strncpy(path, pNode->_path, pNode->_idxLen - 1);
						verifyMysqlFileSystem(NULL, NULL, path, &linkNumsDir, NULL, NULL);
						--linkNumsDir;
						updateMysqlFileSystem(NULL, NULL, path, &linkNumsDir, NULL);
					
						flag = 0;
						printf("Remove the directory successfully!\n");
					}
				} else {
					flag = 2;
					printf("The directory isn't empty!\n");
				}
			} else if (!strcmp("file", fileType)) {
				flag = 8;
			}
		}
	}
	sendN(pNode->_sfdNew, &flag, sizeof(char));
}

void renameFile(pNode_t pNode, const char * oldFileName, const char * newFileName)//未判断当前路径下是否与此文件（目录）
{
	char oldPathName[256] = {0};
	char newPathName[256] = {0};
	sprintf(oldPathName, "%s%s", pNode->_path, oldFileName);
	sprintf(newPathName, "%s%s", pNode->_path, newFileName);
	
	char flag = -1;
	ssize_t ret = verifyMysqlFileSystem(NULL, NULL, oldPathName, NULL, NULL, NULL);
	if (-1 == ret) {
		flag = 1;
		printf("The file or directory doesn't exist!\n");
	} else if (0 == ret) {
		ret = updateMysqlFileSystem(newFileName, newPathName, oldPathName, NULL, NULL);
		if (-1 == ret) {
			flag = -1;
			printf("Rename failed!\n");
		} else if (0 == ret) {
			flag = 0;
			printf("Rename succeeded!\n");
		}
	}
	sendN(pNode->_sfdNew, &flag, sizeof(char));
}

ssize_t signOut(pNode_t pNode)
{
	ssize_t flag = -1;
	sendN(pNode->_sfdNew, (char*)&flag, sizeof(ssize_t));
	return flag;
}
