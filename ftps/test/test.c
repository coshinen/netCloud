 ///
 /// @file    test.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-15 23:36:55
 ///
 
#include "head.h"

int main(void)
{
	char buf[64] = {0};
	scanf("%s", buf);
	buf[strlen(buf) - 1] = 0;
	printf("%s\n", dirname(buf));

	return 0;
}
