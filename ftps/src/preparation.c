 ///
 /// @file    preparation.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-13 11:31:59
 ///

#include "../include/head.h"

char ** readConf(char * argv[])
{
	int fd = open(argv[1], O_RDONLY);
	
	char ** argvConf = (char**)calloc(3,sizeof(char*));
	size_t idx;
	for (idx = 0; idx != 3; ++idx)
	{
		argvConf[idx] = (char*)calloc(1,sizeof(char) * 16);
	}
	
	char buf[1024] = {0};
	read(fd, buf, sizeof(char) * 1024);
	
	for (size_t iConf = 0, jConf = 0, iBuf = 2; iBuf != strlen(buf); ++iBuf)
	{
		if (buf[iBuf - 2] == ' ' && buf[iBuf - 1] == '"') {
			for (; iBuf != strlen(buf); ++iBuf)
			{
				if (buf[iBuf] == '"' && (buf[iBuf + 1] == ',' || buf[iBuf + 1] == '\n')) {
					jConf = 0;
					++iConf;
					break;
				} else {
					argvConf[iConf][jConf] = buf[iBuf];
					++jConf;
				}
			}
		}

		if (3 == iConf) {
			break;
		}
	}
	
	close(fd);
	return argvConf;
}

void getDaemon()
{
	if (fork()) { // 创建子进程，父进程退出
		exit(0);
	}
	
	setsid(); // 设置新会话

	umask(0); // 设置掩码
	
	mkdir("/ftps", 0755); // 创建工作目录

	chdir("/ftps"); // 切换工作目录到/ftps
	
//	for (int idx = 0; idx != 3; ++idx) // 关闭标准输入、输出、错误文件描述符
//	{
//		close(idx);
//	}
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
	strcpy(query, "create table UserInfo(UID smallint unsigned primary key AUTO_INCREMENT, USERNAME char(64) not NULL, PASSWD char(98) not NULL, SALT char(11) not NULL, SIGNUPIP char(16) not NULL, SIGNUPDATE datetime not NULL, SIGNINIP char(16) not NULL, SIGNINDATE datetime not NULL)");
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
