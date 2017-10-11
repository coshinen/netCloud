 ///
 /// @file    test.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-15 23:36:55
 ///
 
#include "head.h"

ssize_t createMysqlUserInfo()
{
	MYSQL mysql;
	mysql_init(&mysql);
	if(!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)){
		mysql_close(&mysql);
		fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
		return -1;
	}else{
		printf("Connected...\n");
	}
	char query[1024] = {0};
	strcpy(query, "create table UserInfo(UID smallint unsigned primary key AUTO_INCREMENT, USERNAME char(64) not NULL, PASSWD char(98) not NULL, SALT char(11) not NULL, SIGNUPIP char(16) not NULL, SIGNUPDATE datetime not NULL, SIGNINIP char(16) not NULL, SIGNINDATE datetime not NULL)");
	puts(query);
	if(mysql_query(&mysql, query)){
		mysql_close(&mysql);
		fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
		return -1;
	}else{
		printf("create table UserInfo succeeded\n");
	}
	mysql_close(&mysql);

	return 0;
}

ssize_t createMysqlFileSystem()
{
	MYSQL mysql;
	mysql_init(&mysql);
	if(!mysql_real_connect(&mysql, "localhost", "root", "2333", "ftps", 0, NULL, 0)){
		mysql_close(&mysql);
		fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
		return -1;
	}else{
		printf("Connected...\n");
	}
	char query[1024] = {0};
	strcpy(query, "create table FileSystem(USERNAME char(64) not NULL, PREINODE mediumint unsigned not NULL, CURINODE mediumint unsigned not NULL unique AUTO_INCREMENT, FILETYPE char(4) not NULL, FILENAME char(64) not NULL, PATH char(255) not NULL, PATHNAME char(255) not NULL primary key, LINKNUMS mediumint unsigned not NULL, MD5 char(32), FILESIZE bigint unsigned not NULL, DATE datetime not NULL, CURSIZE bigint unsigned not NULL)");
	puts(query);
	if(mysql_query(&mysql, query)){
		mysql_close(&mysql);
		fprintf(stderr, "Failed to make query: Error: %s\n", mysql_error(&mysql));
		return -1;
	}else{
		printf("create table FileSystem succeeded\n");
	}
	mysql_close(&mysql);

	return 0;
}

int main(int argc, char * argv[])
{
	ssize_t ret1 = createMysqlUserInfo();
	printf("ret1 = %ld\n", ret1);
	ssize_t ret2 = createMysqlFileSystem();
	printf("ret2 = %ld\n", ret2);

	return 0;
}
