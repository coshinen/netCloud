// Copyright (c) 2017-2019 mistydew
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include "rpcclient.h"

#include <stdio.h>
#include <signal.h>

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
    node._sfd = InitSocket(argv);
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
    ParseCommandStart(cmd);
    char flag = 0;

    int ret;
    if (!strcmp("signup", cmd)) {
        flag = 2;
        sendN(node._sfd, &flag, sizeof(char));
        
        ret = SignUp(node._sfd);
        if (0 == ret) {
            goto Label;
        }
    } else if (!strcmp("signin", cmd)) {
        flag = 1;
        sendN(node._sfd, &flag, sizeof(char));

        ret = VerifySignInInfo(node._sfd);
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
