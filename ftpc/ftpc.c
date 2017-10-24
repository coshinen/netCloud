 ///
 /// @file    ftpc.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-14 22:36:16
 ///

#include "head.h"
char bash[256] = {0};

int main(int argc, char * argv[])
{
	if (argc != 3) {
		printf("Please enter: ./ftpc IP PORT\n");
		return -1;
	}

	signal(SIGPIPE, SIG_IGN);

	Node_t node;
	bzero(&node, sizeof(Node_t));
	node._sfd = scSocket(argv);
Label:
	printf("Type `signin' to Sign in.\n"
			"or `signup' to Sign up.\n"
			"or `signout' to Sign out.\n"
			"$>");
	fflush(stdout);
	char cmd[64] = {0};
	read(0, cmd, sizeof(cmd));
	if (!strcmp("\n", cmd)) {
		goto Label;
	}
	parseCommandStart(cmd);
	char flag = 0;

	ssize_t ret;
	if (!strcmp("signup", cmd)) {
		flag = 2;
		sendN(node._sfd, &flag, sizeof(char));
		
		ret = signUp(node._sfd);
		if (0 == ret) {
			goto Label;
		}
	} else if (!strcmp("signin", cmd)) {
		flag = 1;
		sendN(node._sfd, &flag, sizeof(char));

		ret = verifySignInInfo(node._sfd);
		if (-1 == ret) {
			goto LabelExit;
		} else if (0 == ret) {
			getCommand(&node, argv);
		}
	} else if (!strcmp("signout", cmd)) {
		flag = -1;
		sendN(node._sfd, &flag, sizeof(char));

		goto LabelExit;
	} else {
		goto Label;
	}
LabelExit:
	close(node._sfd);
	return 0;
}
