 ///
 /// @file    test.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-15 23:36:55
 ///
 
#include "head.h"

int main(int argc, char * argv[])
{
	char * p = getpass("hehe");
	puts(p);
	char a[64] = {0};
	strcpy(a, p);
	puts(a);
	char * p1 = getpass("haha");
	puts(p);
	puts(a);
	puts(p1);

	return 0;
}
