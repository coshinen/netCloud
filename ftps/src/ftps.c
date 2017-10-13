 ///
 /// @file    ftps.c
 /// @author  mistydew(mistydew@qq.com)
 /// @date    2017-09-13 11:11:08
 ///

#include "../include/threadHandler.h"

int main(int argc, char * argv[])
{
	if (argc != 2) {
		printf("Please enter: sudo ./ftps ../conf/ftps.conf\n");
		return -1;
	}

	char ** argvConf = readConf(argv);
	for (size_t idx = 0; idx != 3; ++idx)
	{
		printf("%s\n",argvConf[idx]);
	}
	
	getDaemon();

	setExit();
	
	int sfd = sblSocket(argvConf);
	
	createMysqlUserInfo();
	createMysqlFileSystem();

	Factory_t factory;
	bzero(&factory, sizeof(Factory_t));
	size_t numThread = atol(argvConf[2]);
	factoryInit(&factory, numThread, threadHandler);
	factoryStart(&factory);
	
	int epfd = epoll_create(1);
	struct epoll_event ev, * evs =
		(struct epoll_event*)calloc(2, sizeof(struct epoll_event)); // 开辟两个，退出机制
	bzero(&ev, sizeof(struct epoll_event));
	ev.events = EPOLLIN;
	ev.data.fd = sfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev);
	bzero(&ev, sizeof(struct epoll_event));
	ev.events = EPOLLIN;
	ev.data.fd = exitfd[0];
	epoll_ctl(epfd, EPOLL_CTL_ADD, exitfd[0], &ev);
	
	int ret;
	ssize_t idx;
	int sfdNew;
	struct sockaddr_in cli;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	pNode_t pNode;
	while (1)
	{
		ret = epoll_wait(epfd, evs, 2, -1);
		
		for (idx = 0; idx < ret; ++idx)
		{
			if (sfd == evs[idx].data.fd) {
				bzero(&cli, sizeof(struct sockaddr_in));
				sfdNew = accept(sfd, (struct sockaddr*)&cli, &addrlen);
				
				pNode = (pNode_t)calloc(1, sizeof(Node_t));
				pNode->_sfdNew = sfdNew;
				strcpy(pNode->_ip, inet_ntoa(cli.sin_addr));
				pNode->_port = ntohs(cli.sin_port);
				strcpy(pNode->_path, "/ftps/");
				pNode->_idxLen = strlen(pNode->_path);
				pNode->_inum = 0;
				pNode->_lenDir[pNode->_inum] = strlen(pNode->_path);
				
				pthread_mutex_lock(&factory._que._mutex);
				taskQueInsertTail(&factory._que, pNode);
				pthread_mutex_unlock(&factory._que._mutex);
				
				pthread_cond_signal(&factory._cond);
			}

			if (exitfd[0] == evs[idx].data.fd) { // 退出机制可封装
				bzero(&ev, sizeof(struct epoll_event));
				ev.events = EPOLLIN;
				ev.data.fd = sfd;
				epoll_ctl(epfd, EPOLL_CTL_DEL, sfd, &ev);
				close(sfd);
			
				for (size_t idx = 0; idx != numThread; ++idx)
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
	
	return 0;
}
