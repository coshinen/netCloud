 ///
 /// @file    threadHandler.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-14 21:20:12
 ///
 
#include "../include/threadHandler.h"

void cleanupCondMutex(void * p)
{
	pthread_mutex_t * mutex = (pthread_mutex_t*)p;
	pthread_mutex_unlock(mutex);
}

void cleanup(void * p)
{
	pNode_t pCur = (pNode_t)p;
	close(pCur->_sfdNew);
	free(pCur);
}

void * threadHandler(void * p)
{
	openlog(0, LOG_CONS | LOG_PID, LOG_LOCAL0);
	
	pFactory_t pFactory = (pFactory_t)p;
	pQue_t pQue = &pFactory->_que;
	
	pNode_t pCur;
	char flag;
	ssize_t ret;
	while (1)
	{
		pthread_cleanup_push(cleanupCondMutex, &pQue->_mutex);
		pthread_mutex_lock(&pQue->_mutex);
		if (NULL == pQue->_pHead) {
			pthread_cond_wait(&pFactory->_cond, &pQue->_mutex);
		}
		taskQueGet(pQue, &pCur);
		pthread_mutex_unlock(&pQue->_mutex);
		pthread_cleanup_pop(0);
		
		pthread_cleanup_push(cleanup, pCur);
		
Label:
		ret = recvN(pCur->_sfdNew, &flag, sizeof(char));
		if (-1 == ret || 0 == ret) {
			syslog(LOG_INFO, "[user] %s [info] %s\n", pCur->_user, "Disconnected!");
			free(pCur);
			printf("Disconnected!\n");
			continue;
		}
		
		if (2 == flag) { // Sign up
			ret = signUp(pCur);
			if (-1 == ret) {
				free(pCur);
				continue;
			} else if (0 == ret) {
				goto Label;
			}
		} else if (1 == flag) { // Sign in
			ret = verifySignInInfo(pCur);
			if (-1 == ret) {
				syslog(LOG_INFO, "[user] %s [info] %s\n", pCur->_user, "Disconnected!");
				free(pCur);
				printf("Disconnected!\n");
			} else if (0 == ret) { // Sign in succeeded
				getCommand(pCur);
				syslog(LOG_INFO, "[user] %s [info] %s\n", pCur->_user, "Sign out");
				free(pCur);
				printf("Sign out\n");
			}
		} else if (-1 == flag) { // Sign out
			free(pCur);
			printf("Sign out\n");
		}
		
		pthread_cleanup_pop(0);
	}

	closelog();
}

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

ssize_t verifyMysqlUsername(const char * userName, char * passwd, char * salt, char * signupDate, char * signinIp, char * signinDate)
{
	ssize_t flag = -1;
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
					if (passwd != NULL && salt != NULL) {
						strcpy(passwd, row[2]);
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

ssize_t insertMysqlUserInfo(const char * userName, const char * passwd, const char * salt, const char * signupIp, const char * signupDate, const char * signinIp, const char * signinDate)
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

ssize_t updateMysqlUserInfo(const char * userName, const char * signinIp, const char * signinDate)
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

ssize_t verifyMysqlFileSystem(size_t * curInode, char * fileType, char * pathName, size_t * linkNums, char * md5, size_t * fileSize)
{
	ssize_t flag = -1;
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
						*curInode = (size_t)atol(row[2]);
					}
					if (fileType != NULL && !strcmp(row[6], pathName)) {
						strcpy(fileType, row[3]);
					}
					if (linkNums != NULL && !strcmp(row[6], pathName)) {
						*linkNums = (size_t)atol(row[7]);
					}
					if (md5 != NULL && !strcmp(row[6], pathName)) {
						strcpy(md5, row[8]);
					}
					if (fileSize != NULL && !strcmp(row[6], pathName)) {
						*fileSize = (size_t)atol(row[9]);
					}
					if (!strcmp(row[6], pathName)) {
						flag = 0;
						break;
					}
				}
				if (md5 != NULL && !strcmp(row[8], md5) && fileSize != NULL && linkNums != NULL) {
					*linkNums = (size_t)atol(row[7]);
					*fileSize = (size_t)atol(row[9]);
					
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

ssize_t insertMysqlFileSystem(const char * userName, size_t preInode, const char * fileType, const char * fileName, const char * path, const char * pathName, size_t linkNums, const char * md5, size_t fileSize, const char * date, size_t curSize)
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

ssize_t updateMysqlFileSystem(const char * newFileName, const char * newPathName, const char * oldPathName, const size_t * linkNums, const char * md5)
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

ssize_t deleteMysqlFileSystem(const char * pathName)
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

ssize_t signUp(pNode_t pNode)
{
	ssize_t ret;
	char flag;
	Train_t train;
Label:
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

	ret = verifyMysqlUsername(username, NULL, NULL, NULL, NULL, NULL);
	if (0 == ret) {
		flag = -1;
		sendN(pNode->_sfdNew, &flag, sizeof(char));
		goto Label;
	}
	sendN(pNode->_sfdNew, &flag, sizeof(char));
	
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

//	char signupIp[16] = {0};
//	recvN(pNode->_sfdNew, signupIp, sizeof(signupIp));
	char signupDate[20] = {0};
	time_t t;
	time(&t);
	struct tm * pgm = gmtime(&t);
	sprintf(signupDate, "%d-%02d-%02d %02d:%02d:%02d", 1900 + pgm->tm_year, 1 + pgm->tm_mon, pgm->tm_mday, 8 + pgm->tm_hour, pgm->tm_min, pgm->tm_sec);

	ret = insertMysqlUserInfo(username, passwd, salt, pNode->_ip, signupDate, pNode->_ip, signupDate);
	if (-1 == ret) {
		flag = -1;
		sendN(pNode->_sfdNew, &flag, sizeof(char));
		goto Label;
	} else if (0 == ret) {
		flag = 0;
		sendN(pNode->_sfdNew, &flag, sizeof(char));
	}

	return 0;
}

ssize_t verifySignInInfo(pNode_t pNode)
{
	char flag;
	ssize_t ret;
LabelUsername:
	flag = 0;
	char username[64] = {0};
	Train_t train;
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

	char passwd[99] = {0}; // 加密密码98位
	char salt[12] = {0}; // 盐值11位
	ret = verifyMysqlUsername(username, passwd, salt, NULL, NULL, NULL);
	if (-1 == ret) {
		flag = -1;
		sendN(pNode->_sfdNew, &flag, sizeof(char));
		goto LabelUsername;
	}
	sendN(pNode->_sfdNew, &flag, sizeof(char));

	bzero(&train, sizeof(Train_t));
	strcpy(train._buf, salt);
	train._len = strlen(train._buf);
	sendN(pNode->_sfdNew, (char*)&train, sizeof(size_t) + train._len);
	
	char password[99] = {0};
LabelPassword:	
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
		ret = verifyMysqlUsername(username, NULL, NULL, signupDate, signinIp, signinDate);
		if (-1 == ret) {
			flag = -1;
			sendN(pNode->_sfdNew, &flag, sizeof(char));
			printf("Verification failed!\n");
			goto LabelPassword;
		} else if (0 == ret) {
			if (!strcmp(signupDate, signinDate)) {
				char pathName[256] = {0};
				sprintf(pathName, "%s%s", pNode->_path, username);
				char date[20] = {0};
				getDate(date);
				insertMysqlFileSystem(username, 0, "dir", username, pNode->_path, pathName, 0, NULL, 4096, date, 4096);
			}
			
			flag = 0;
			sendN(pNode->_sfdNew, &flag, sizeof(char));
	
			sendN(pNode->_sfdNew, signinIp, sizeof(signinIp));
			sendN(pNode->_sfdNew, signinDate, sizeof(signinDate));
			
//			recvN(pNode->_sfdNew, signinIp, sizeof(signinIp));
			time_t t;
			time(&t);
			struct tm * pgm = gmtime(&t);
			sprintf(signinDate, "%d-%02d-%02d %02d:%02d:%02d", 1900 + pgm->tm_year, 1 + pgm->tm_mon, pgm->tm_mday, 8 + pgm->tm_hour, pgm->tm_min, pgm->tm_sec);
			ret = updateMysqlUserInfo(username, pNode->_ip, signinDate);
			if (-1 == ret) {
				flag = -1;
			
				sendN(pNode->_sfdNew, &flag, sizeof(char));
				printf("Verification failed!\n");
				goto LabelPassword;
			}

			strcpy(pNode->_user, username);
			syslog(LOG_INFO, "[user] %s [info] %s\n", pNode->_user, "Sign in");

			sprintf(pNode->_path + pNode->_idxLen, "%s%s", username, "/");
			pNode->_idxLen = strlen(pNode->_path);
			pNode->_lenDir[pNode->_inum] = strlen(pNode->_path);

			printf("Verification passed!\n");
		}
	} else {
		flag = -1;
		
		sendN(pNode->_sfdNew, &flag, sizeof(char));
		printf("Verification failed!\n");
		goto LabelPassword;
	}

	return 0;
}
